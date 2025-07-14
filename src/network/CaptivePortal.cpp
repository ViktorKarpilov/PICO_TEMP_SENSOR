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

    const auto dhcp_server = new DhcpServer();
    if (dhcp_server ->init(CONFIG::DNS_ADDRESS, CONFIG::NETMASK, CONFIG::DHCP_BASE_IP) != 0) {
        printf("Failed to initialize DHCP server\n");
        return -1;
    }

    const auto dns_server = new DnsServer();
    if (dns_server->init())
    {
        printf("Failed to initialize DNS server\n");
        return -1;
    }

    auto http_server = HttpServer();
    http_server.init(CONFIG::WEB_SERVER_PORT);

    printf("Captive portal is live!\n");

    return 0;
}
