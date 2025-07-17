#include <string>
#include "DnsServer.h"
#include "DnsTable.h"
#include <unordered_map>
#include <src/config.h>

std::unordered_map<std::string, ip4_addr_t> ip4_addresses
{
    {"google.com", make_ip4_addr(8, 8, 8, 8)},
    {"google-alt.com", make_ip4_addr(8, 8, 4, 4)},
    {"admin.com", CONFIG::AP_IP},
    {"config.com", CONFIG::AP_IP},
    {"portal.com", CONFIG::AP_IP},
    {"login.com", CONFIG::AP_IP},
};

uint32_t ip4_addr_to_uint32(const ip4_addr_t* ip)
{
    return (ip4_addr1(ip) << 24) | (ip4_addr2(ip) << 16) |
        (ip4_addr3(ip) << 8) | ip4_addr4(ip);
}
