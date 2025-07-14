#ifndef DNSTABLE_H
#define DNSTABLE_H

#include <unordered_map>
#include <string>

constexpr ip4_addr_t make_ip4_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    ip4_addr_t addr;
    IP4_ADDR(&addr, a, b, c, d);
    return addr;
}

extern std::unordered_map<std::string, ip4_addr_t> ip4_addresses;
uint32_t ip4_addr_to_uint32(const ip4_addr_t* ip);

#endif //DNSTABLE_H
