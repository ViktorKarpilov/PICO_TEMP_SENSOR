//
// Created by PC on 7/19/2025.
//

#include "WifiService.h"

#include <cstring>
#include <pico/cyw43_arch.h>
#include <src/network/dhcpserver/DhcpServer.h>
#include <src/network/dnsserver/DnsServer.h>
#include <src/network/http_server/HttpServer.h>

struct wifi_state
{
    WifiService *self;
    wifi_mode next_mode = Offline;

    DhcpServer *dhcp = new DhcpServer();
    DnsServer *dns= new DnsServer();
    HttpServer *http{};

    uint8_t ssids_list[50][32]{};
    int ssids_count = 0;

    std::string user_network_pass;
    std::string user_network_ssid;
};

WifiService::WifiService()
{
    this->mode = Offline;
    this->state = new wifi_state();
    this->state->http = new HttpServer(this);
    this->state->self = this;
}
WifiService::~WifiService() = default;


int scan_result(void* env, const cyw43_ev_scan_result_t* result)
{
    const auto state = static_cast<wifi_state*>(env);

    if (result == nullptr) {
        state->self->mode = state->next_mode;
        state->next_mode = Offline;
        return 0;
    }
    printf("SSID During scan: %s\n", reinterpret_cast<const char*>(result->ssid));

    if (state->ssids_count < 50) {
        memcpy(state->ssids_list[state->ssids_count], result->ssid, result->ssid_len);
        state->ssids_count++;
    }

    return 0;
}

int WifiService::discover_identifiers()
{
    this->state->next_mode = this->mode;
    this->mode = WifiScanningMode;
    cyw43_wifi_scan_options_t scan_options = {};

    if (cyw43_wifi_scan(&cyw43_state, &scan_options, this->state, scan_result))
    {
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

    printf("Enable ap mode\n");
    cyw43_arch_enable_ap_mode(CONFIG::AP_NAME, CONFIG::PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    printf("ap initiated\n");

    if (this->state->dhcp->init(CONFIG::DNS_ADDRESS, CONFIG::NETMASK, CONFIG::DHCP_BASE_IP)) {
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

int WifiService::get_ssids(uint8_t ssids[][32], int max_count) const {
    const int actual_count = std::min(max_count, 50);
    
    for(int i = 0; i < actual_count; i++) {
        printf("SSID[%d]: %s\n", i, reinterpret_cast<char*>(ssids[i]));
        std::memcpy(ssids[i], this->state->ssids_list[i], 32);
    }
    
    return actual_count;
}
