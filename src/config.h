#ifndef CONFIG_H
#define CONFIG_H
#include <cstdint>
#include <lwip/ip4_addr.h>

#include "hardware/i2c.h"
#include "network/dnsserver/DnsTable.h"

namespace CONFIG
{
    constexpr int V3V_PIN  = 36;
    constexpr int UART_TX = 0;
    constexpr int UART_RX = 1;

    // Diode configs
    constexpr int RED_DIODE = 2;
    constexpr int GREEN_DIODE = 5;
    constexpr int SYSTEM_DIODE = 0;
    constexpr int DIODE_SLEEP_DURATION = 1000;

    // pico i2c config
    constexpr i2c_inst* I2CChip = i2c0;
    constexpr int SDA_PIN = 20;
    constexpr int SCL_PIN = 21;

    // pico for SSD1306 config
    constexpr int VCC_PIN  = V3V_PIN;
    constexpr int SSD1306_GND = 23;

    // SSD1306 config
    constexpr uint8_t SSD1306_ADDR = 0x3C;

    // WiFi AP config
    constexpr const char* AP_NAME = "picow_test";
    constexpr const char* PASSWORD = nullptr;
    constexpr ip4_addr_t AP_IP = make_ip4_addr(7, 7, 7, 7);

    // DNS config
    constexpr ip4_addr_t DNS_ADDRESS = AP_IP;
    constexpr ip4_addr_t NETMASK = make_ip4_addr(255, 255, 255, 0);
    constexpr int DNS_PORT = 53;
    #define DSN_DEBUG 0

    // DHCP server
    constexpr int DHCP_SERVER_PORT = 67;
    constexpr int DHCP_CLIENT_PORT = 68;
    constexpr uint DEFAULT_LEASE_TIME_S = 24 * 60 * 60; // 24 hours
    // constexpr uint DEFAULT_LEASE_TIME_S = 30; // 30 secodns
    constexpr uint MAC_ADDRESS_LEN = 6;
    constexpr uint MAX_DHCP_CLIENTS = 10;
    constexpr std::uint8_t DHCP_BASE_IP = 8;
    #define DHCP_DEBUG 0

    // WEB SERVER CONFIG
    constexpr int WEB_SERVER_PORT = 80;
}

#endif //CONFIG_H
