#ifndef DNSTABLE_H
#define DNSTABLE_H

#include <unordered_map>
#include <string>
#include <hardware/network/network.h>

constexpr sensor_ip4_addr_t make_ip4_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    sensor_ip4_addr_t addr;
    sensor_IP4_ADDR(&addr, a, b, c, d);
    return addr;
}

extern std::unordered_map<std::string, sensor_ip4_addr_t> ip4_addresses;

#endif //DNSTABLE_H
