#include <string>
#include "DnsServer.h"
#include "DnsTable.h"
#include <unordered_map>
#include <src/config.h>

std::unordered_map<std::string, sensor_ip4_addr_t> ip4_addresses
{
    {"google.com", make_ip4_addr(8, 8, 8, 8)},
    {"google-alt.com", make_ip4_addr(8, 8, 4, 4)},
    {"admin.com", CONFIG::AP_IP},
    {"config.com", CONFIG::AP_IP},
    {"portal.com", CONFIG::AP_IP},
    {"login.com", CONFIG::AP_IP},
};
