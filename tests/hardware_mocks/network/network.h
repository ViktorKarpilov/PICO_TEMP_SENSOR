//
// Created by PC on 7/26/2025.
//

#ifndef NETWORK_H
#define NETWORK_H
#include <cstdint>


// ===== TYPE ALIASES =====
using tcp_pcb_control_block = tcp_pcb;
using udp_pcb_control_block = udp_pcb;
using sensor_udp_recv_fn = udp_recv_fn;
using sensor_ip4_addr_t = ip4_addr_t;
using sensor_ip_addr_t = ip_addr_t;
using sensor_pbuf = pbuf;
using sensor_err_t = err_t;
using sensor_u16_t = u16_t;
using sensor_uint16_t = uint16_t;
using sensor_uint32_t = uint32_t;
using sensor_uint8_t = uint8_t;

// ===== FUNCTION POINTER ALIASES =====
// Note: For functions, you need function pointers, not direct aliases
using sensor_udp_new_fn = udp_pcb* (*)();
using sensor_udp_recv_fn_ptr = void (*)(udp_pcb*, udp_recv_fn, void*);
using sensor_udp_bind_fn = err_t (*)(udp_pcb*, const ip_addr_t*, u16_t);
using sensor_udp_sendto_fn = err_t (*)(udp_pcb*, pbuf*, const ip_addr_t*, u16_t);
using sensor_udp_disconnect_fn = void (*)(udp_pcb*);
using sensor_pbuf_alloc_fn = pbuf* (*)(pbuf_layer, u16_t, pbuf_type);
using sensor_pbuf_free_fn = u8_t (*)(pbuf*);

// ===== CONSTANTS ALIASES =====
constexpr auto SENSOR_PBUF_TRANSPORT = PBUF_TRANSPORT;
constexpr auto SENSOR_PBUF_RAM = PBUF_RAM;
constexpr auto SENSOR_ERR_OK = ERR_OK;
constexpr auto SENSOR_ENOMEM = ENOMEM;
constexpr auto SENSOR_IP_ANY_TYPE = IP_ANY_TYPE;

// ===== FUNCTION ALIASES =====
#define sensor_udp_new() udp_new()
#define sensor_udp_recv(pcb, recv, recv_arg) udp_recv(pcb, recv, recv_arg)
#define sensor_udp_bind(pcb, ipaddr, port) udp_bind(pcb, ipaddr, port)
#define sensor_udp_sendto(pcb, p, dst_ip, dst_port) udp_sendto(pcb, p, dst_ip, dst_port)
#define sensor_udp_disconnect(pcb) udp_disconnect(pcb)
#define sensor_pbuf_copy_partial(packet, request, size, start) pbuf_copy_partial(packet, request, size, start)
#define sensor_pbuf_alloc(layer, length, type) pbuf_alloc(layer, length, type)
#define sensor_pbuf_free(p) pbuf_free(p)
#define sensor_htons(n) htons(n)
#define sensor_htonl(n) htonl(n)
#define sensor_ip4addr_ntoa(addr) ip4addr_ntoa(addr)
#define sensor_ip_2_ip4(ipaddr) ip_2_ip4(ipaddr)
#define sensor_IP4_ADDR(addr, a, b, c, d) IP4_ADDR(addr, a, b, c, d)
#define sensor_cyw43_hal_ticks_ms() cyw43_hal_ticks_ms()
#define sensor_udp_remove(udp) udp_remove(udp)

// Mock versions of the macros
#define sensor_ip4_addr1(ipaddr) ((uint8_t)(((ipaddr)->addr) & 0xff))
#define sensor_ip4_addr2(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 8) & 0xff))
#define sensor_ip4_addr3(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 16) & 0xff))
#define sensor_ip4_addr4(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 24) & 0xff))

inline uint32_t sensor_ip4_addr_to_uint32(const sensor_ip4_addr_t* ip)
{
    return (sensor_ip4_addr1(ip) << 24) | (sensor_ip4_addr2(ip) << 16) |
        (sensor_ip4_addr3(ip) << 8) | sensor_ip4_addr4(ip);
}

#endif //NETWORK_H
