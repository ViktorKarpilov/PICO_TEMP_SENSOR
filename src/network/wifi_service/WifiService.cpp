//
// Created by PC on 7/19/2025.
//

#include "WifiService.h"

#include <pico/cyw43_arch.h>
#include <src/network/dhcpserver/DhcpServer.h>
#include <src/network/dnsserver/DnsServer.h>
#include <src/network/http_server/HttpServer.h>

#include "hardware/stdio/stdio.h"

enum wifi_mode
{
    Offline = 0,
    CaptivePortalMode,
    StationMode,
    WifiScanningMode,
};

struct wifi_state
{
    wifi_mode mode = Offline;
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
    this->state = new wifi_state();
    this->state->http = new HttpServer(this);
}
WifiService::~WifiService() = default;


int scan_result(void* env, const cyw43_ev_scan_result_t* result)
{
    const auto state = static_cast<wifi_state*>(env);

    if (result == nullptr) {
        state->mode = state->next_mode;
        state->next_mode = Offline;
        return 0;
    }

    if (state->ssids_count < 50) {
        memcpy(state->ssids_list[state->ssids_count], result->ssid, result->ssid_len);
        state->ssids_count++;
    }

    return 0;
}

int WifiService::discover_identifiers() const
{
    this->state->next_mode = this->state->mode;
    this->state->mode = WifiScanningMode;
    cyw43_wifi_scan_options_t scan_options = {};

    if (cyw43_wifi_scan(&cyw43_state, &scan_options, this->state, scan_result))
    {
        return ERR_CONN;
    }

    return ERR_OK;
}

int WifiService::turn_on_captive_portal() const
{
    if (this->state->mode == CaptivePortalMode)
    {
        return ERR_OK;
    }

    log("Enable ap mode\n");
    cyw43_arch_enable_ap_mode(CONFIG::AP_NAME, CONFIG::PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    log("ap initiated\n");

    if (this->state->dhcp->init(CONFIG::DNS_ADDRESS, CONFIG::NETMASK, CONFIG::DHCP_BASE_IP)) {
        log("Failed to initialize DHCP server\n");
        return ERR_CONN;
    }

    if (this->state->dns->init())
    {
        log("Failed to initialize DNS server\n");
        return ERR_RTE;
    }

    if (this->state->http->init(CONFIG::WEB_SERVER_PORT))
    {
        log("Failed to initialize http server\n");
        return -1;
    }

    cyw43_arch_lwip_begin();
    netif_set_addr(netif_default, &CONFIG::AP_IP, &CONFIG::NETMASK, &CONFIG::AP_IP);
    cyw43_arch_lwip_end();

    this->state->mode = CaptivePortalMode;

    return ERR_OK;
}
