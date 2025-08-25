//
// Created by PC on 7/19/2025.
//

#include "WifiService.h"

#include <algorithm>
#include <cstring>
#include <pico/cyw43_arch.h>
#include <src/network/dhcpserver/DhcpServer.h>
#include <src/network/dnsserver/DnsServer.h>
#include <src/network/http_server/HttpServer.h>
#include "src/network/udp_server/UdpServer.h"

#include "src/src.h"
#include "src/enviroment_sensor/enviroment_sensor.h"
#include "src/network/mqtt_server/MQTTServer.h"

using namespace std;

struct wifi_state
{
    wifi_mode next_mode = Offline;

    DhcpServer* dhcp = new DhcpServer();
    DnsServer* dns = new DnsServer();
    HttpServer* http{};
    UdpServer* udp = nullptr;
    MQTTServer* mqtt = nullptr;

    vector<string> ssids_list;
    int ssids_count = 0;

    std::string user_network_pass;
    std::string user_network_ssid;

    uint32_t scan_start_time = 0;
    bool scan_timeout_reached = false;
};

WifiService::WifiService()
{
    this->mode = Offline;
    printf("(Constructor) Set mode: %d\n", this->mode);
    this->state = new wifi_state();
    this->state->http = new HttpServer();
}

WifiService::~WifiService()
{
    if (state)
    {
        delete state->dhcp;
        delete state->dns;
        delete state->http;
        delete state->udp;
        delete state;
    }
}

int scan_result(void* env, const cyw43_ev_scan_result_t* result)
{
    const auto state = static_cast<wifi_state*>(env);

    if (result == nullptr)
    {
        printf("(scan_result) Set mode: %d\n", state->next_mode);
        get_wifi_service().mode = state->next_mode;
        state->next_mode = Offline;
        return 0;
    }

    if (result->ssid_len == 0 || result->ssid_len > 32)
    {
        printf("Invalid SSID length: %d\n", result->ssid_len);
        return 0;
    }

    if (state->ssids_count < 50)
    {
        std::string current_ssid(reinterpret_cast<const char*>(result->ssid), result->ssid_len);

        auto it = ranges::find(state->ssids_list, current_ssid);
        if (it == state->ssids_list.end())
        {
            state->ssids_list.emplace_back(current_ssid);
            state->ssids_count++;
        }
    }

    return 0;
}

uint32_t WifiService::get_scan_start_time() const
{
    return this->state->scan_start_time;
}

void WifiService::force_scan_completion()
{
    this->mode = this->state->next_mode;
    this->state->next_mode = Offline;
    this->state->scan_timeout_reached = true;
}

int WifiService::discover_identifiers()
{
    this->state->next_mode = this->mode;
    this->state->ssids_count = 0;

    this->state->ssids_list.clear();

    this->state->scan_timeout_reached = false;
    this->state->scan_start_time = to_ms_since_boot(get_absolute_time());

    this->mode = WifiScanningMode;
    cyw43_wifi_scan_options_t scan_options = {};
    scan_options.scan_type = 0;

    if (cyw43_wifi_scan(&cyw43_state, &scan_options, this->state, scan_result))
    {
        printf("Failed to start WiFi scan\n");
        return ERR_CONN;
    }

    return ERR_OK;
}

int WifiService::turn_on_captive_portal()
{
    if (this->mode == CaptivePortalMode)
    {
        return ERR_OK;
    }

    cyw43_arch_enable_ap_mode(CONFIG::AP_NAME, CONFIG::PASSWORD, CYW43_AUTH_WPA2_AES_PSK);

    if (this->state->dhcp->init(CONFIG::DNS_ADDRESS, CONFIG::NETMASK, CONFIG::DHCP_BASE_IP))
    {
        printf("Failed to initialize DHCP server\n");
        return ERR_CONN;
    }

    if (this->state->dns->init())
    {
        printf("Failed to initialize DNS server\n");
        return ERR_RTE;
    }

    if (this->state->http->init(CONFIG::WEB_SERVER_PORT))
    {
        printf("Failed to initialize http server\n");
        return -1;
    }

    cyw43_arch_lwip_begin();
    netif_set_addr(netif_default, &CONFIG::AP_IP, &CONFIG::NETMASK, &CONFIG::AP_IP);
    cyw43_arch_lwip_end();

    this->mode = CaptivePortalMode;

    return ERR_OK;
}

std::vector<std::string> WifiService::get_ssids() const
{
    std::vector<std::string> ssids;
    ssids.reserve(this->state->ssids_count);
    for (int i = 0; i < this->state->ssids_count; i++)
    {
        ssids.push_back(this->state->ssids_list[i]);
    }

    return ssids;
}

int WifiService::schedule_connect_user_network(const std::string& ssid, const std::string& password)
{
    printf("Scheduling connect user network\n");
    this->state->user_network_pass = password;
    this->state->user_network_ssid = ssid;
    this->mode = ScheduleStationMode;
    return ERR_OK;
}

int WifiService::connect_user_network()
{
    printf("Shutting down AP services...\n");
    if (this->state->dhcp)
    {
        this->state->dhcp->deinit();
    }
    if (this->state->dns)
    {
        this->state->dns->deinit();
    }
    if (this->state->http)
    {
        this->state->http->deinit();
    }

    // Disable AP mode and wait a bit
    printf("Disabling AP mode...\n");
    cyw43_arch_disable_ap_mode();
    sleep_ms(1000);

    printf("Resetting network interface...\n");
    cyw43_arch_lwip_begin();
    netif_set_addr(netif_default, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    cyw43_arch_lwip_end();

    sleep_ms(500);

    printf("Enabling station mode...\n");
    cyw43_arch_enable_sta_mode();
    sleep_ms(500);

    printf("Connecting to WiFi...\n");
    const int result = cyw43_arch_wifi_connect_timeout_ms(
        this->state->user_network_ssid.c_str(),
        this->state->user_network_pass.c_str(),
        CYW43_AUTH_WPA2_AES_PSK,
        30000
    );

    if (result == 0)
    {
        printf("WiFi connection successful!\n");
        printf("IP address: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

        this->mode = StationMode;
        this->state->udp = new UdpServer();
        return ERR_OK;
    }

    printf("WiFi connection failed with error: %d\n", result);
    this->mode = RecoveringMode;

    return ERR_CONN;
}

void WifiService::ping()
{
    switch (this->mode)
    {
    case StationMode:
        {
            if (!this->state->udp)
            {
                this->state->udp = new UdpServer();
                break;
            }

            if (!this->state->udp->state->mqtt_broker_addr)
            {
                this->state->udp->get_mqtt_broker_ip();
                break;
            }

            if (!this->state->mqtt)
            {
                this->state->mqtt = new MQTTServer(&this->state->udp->state->mqtt_broker_addr.value());
                break;
            }

            this->state->mqtt->publish_weather_data(EnvironmentSensor::readPressure(),
                                                    EnvironmentSensor::readTemperature(),
                                                    EnvironmentSensor::readHumidity());
            break;
        }
    case ScheduleStationMode:
        connect_user_network();
        break;
    default:
        break;
    }
}
