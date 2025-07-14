//
// Created by PC on 7/14/2025.
//
#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <array>
#include <cstdint>
#include <lwip/udp.h>
#include <src/config.h>

#include "lwip/ip_addr.h"

// DHCP Message Types
enum class DhcpMessageType : std::uint8_t {
    DISCOVER = 1,
    OFFER = 2,
    REQUEST = 3,
    DECLINE = 4,
    ACK = 5,
    NACK = 6,
    RELEASE = 7,
    INFORM = 8
};

// DHCP Options
enum class DhcpOption : std::uint8_t {
    PAD = 0,
    SUBNET_MASK = 1,
    ROUTER = 3,
    DNS = 6,
    HOST_NAME = 12,
    REQUESTED_IP = 50,
    IP_LEASE_TIME = 51,
    MSG_TYPE = 53,
    SERVER_ID = 54,
    PARAM_REQUEST_LIST = 55,
    MAX_MSG_SIZE = 57,
    VENDOR_CLASS_ID = 60,
    CLIENT_ID = 61,
    END = 255
};

struct dhcp_message_t {
    std::uint8_t op;           // message opcode
    std::uint8_t htype;        // hardware address type  
    std::uint8_t hlen;         // hardware address length
    std::uint8_t hops;
    std::uint32_t xid;         // transaction id
    std::uint16_t secs;        // client seconds elapsed
    std::uint16_t flags;
    std::uint8_t ciaddr[4];    // client IP address
    std::uint8_t yiaddr[4];    // your IP address
    std::uint8_t siaddr[4];    // next server IP address
    std::uint8_t giaddr[4];    // relay agent IP address  
    std::uint8_t chaddr[16];   // client hardware address
    std::uint8_t sname[64];    // server host name
    std::uint8_t file[128];    // boot file name
    std::uint8_t options[312]; // optional parameters
};

struct dhcp_lease_t {
    std::array<std::uint8_t, CONFIG::MAC_ADDRESS_LEN> mac;
    std::uint32_t expiry;
    bool is_active;
    
    dhcp_lease_t() : mac{}, expiry(0), is_active(false) {}
};

class DhcpServer {
public:
    explicit DhcpServer();
    ~DhcpServer();
    
    int init(const ip4_addr_t& server_ip, const ip4_addr_t& netmask, std::uint8_t base_ip = 8);
    void deinit();
    
private:
    udp_pcb* control_block;
    ip4_addr_t server_ip;
    ip4_addr_t netmask;
    std::uint8_t base_ip_offset;
    std::array<dhcp_lease_t, CONFIG::MAX_DHCP_CLIENTS> leases;
    
    static udp_recv_fn udp_process_request_function;
    
    // Helper functions
    std::uint8_t* find_dhcp_option(std::uint8_t* options, DhcpOption option_type);
    void write_dhcp_option_u8(std::uint8_t** opt, DhcpOption option, std::uint8_t value);
    void write_dhcp_option_u32(std::uint8_t** opt, DhcpOption option, std::uint32_t value);
    void write_dhcp_option_bytes(std::uint8_t** opt, DhcpOption option, std::size_t len, const void* data);
    
    int find_available_lease(const std::uint8_t* client_mac);
    bool is_mac_equal(const std::uint8_t* mac1, const std::uint8_t* mac2);
    bool is_mac_empty(const std::uint8_t* mac);
    bool is_lease_expired(const dhcp_lease_t& lease);
    
    pbuf* create_dhcp_response(const dhcp_message_t& request, DhcpMessageType response_type, std::uint8_t client_ip_offset);
    void process_dhcp_discover(const dhcp_message_t& request, const ip_addr_t* client_addr, std::uint16_t client_port);
    void process_dhcp_request(const dhcp_message_t& request, const ip_addr_t* client_addr, std::uint16_t client_port);
};

#endif //DHCPSERVER_H
