//
// Created by PC on 7/12/2025.
//

#include "CaptivePortal.h"
#include <pico/cyw43_arch.h>
#include <pico/stdio.h>
#include <src/config.h>

#include "access_point/dhcpserver/DhcpServer.h"
#include "access_point/dnsserver/DnsServer.h"
#include "http_server/HttpServer.h"

CaptivePortal::CaptivePortal()
= default;

int CaptivePortal::initiate()
{
    printf("Enable ap mode\n");
    cyw43_arch_enable_ap_mode(CONFIG::AP_NAME, CONFIG::PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    printf("ap initiated\n");

    cyw43_arch_lwip_begin();
    netif_set_addr(netif_default, &CONFIG::AP_IP, &CONFIG::NETMASK, &CONFIG::AP_IP);
    cyw43_arch_lwip_end();

    printf("=== NETWORK INTERFACE CHECK ===\n");
    cyw43_arch_lwip_begin(); 
    cyw43_arch_lwip_end();

    if (this->dhcpServer.init(CONFIG::DNS_ADDRESS, CONFIG::NETMASK, CONFIG::DHCP_BASE_IP) != 0) {
        printf("Failed to initialize DHCP server\n");
        return -1;
    }

    if (this->dnsServer.init())
    {
        printf("Failed to initialize DNS server\n");
        return -1;
    }

    HttpServer::test_can_bind();
    if (this->httpServer.init(CONFIG::WEB_SERVER_PORT))
    {
        printf("Failed to initialize http server\n");
        return -1;
    }

    printf("Captive portal is live!\n");

    return 0;
}
