//
// Created by PC on 7/19/2025.
//

#include "WifiService.h"

#include <pico/cyw43_arch.h>
#include <src/network/captive_portal/CaptivePortal.h>
#include <src/network/dhcpserver/DhcpServer.h>
#include <src/network/dnsserver/DnsServer.h>

enum wifi_mode
{
    Offline = 0,
    CaptivePortalMode,
    StationMode
};

struct wifi_state
{
    wifi_mode mode = Offline;
    CaptivePortal *portal = new CaptivePortal();
    DhcpServer *dhcp = new DhcpServer();
    DnsServer *dns= new DnsServer();
    HttpServer *http = new HttpServer();

    std::string user_network_pass;
    std::string user_network_ssid;
};

WifiService::WifiService()
{
    this->state = new wifi_state();
}
WifiService::~WifiService() = default;

int WifiService::turn_on_captive_portal()
{
    if (this->state->mode == CaptivePortalMode)
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

    this->state->mode = CaptivePortalMode;

    return ERR_OK;
}
