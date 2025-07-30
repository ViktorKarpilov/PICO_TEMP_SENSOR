//
// Created by PC on 7/14/2025.
//
#include "DhcpServer.h"
#include <cstring>
#include <cstdio>

constexpr std::uint32_t DHCP_MAGIC_COOKIE = 0x63825363;
constexpr std::uint8_t DHCP_BOOTREPLY = 2;
constexpr std::uint8_t DHCP_HTYPE_ETHERNET = 1;
constexpr std::size_t DHCP_MIN_PACKET_SIZE = 240 + 3;

DhcpServer::DhcpServer() 
    : control_block(nullptr), server_ip{}, netmask{}, base_ip_offset(8), leases{} 
{
}

DhcpServer::~DhcpServer() {
    deinit();
}

int DhcpServer::init(const sensor_ip4_addr_t& server_ip, const sensor_ip4_addr_t& netmask, std::uint8_t base_ip) {
    this->server_ip = server_ip;
    this->netmask = netmask;
    this->base_ip_offset = base_ip;
    
    control_block = sensor_udp_new();
    if (control_block == nullptr) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Failed to create UDP PCB\n");
#endif
        return -1;
    }
    
    sensor_udp_recv(control_block, udp_process_request_function, this);
    
    // Bind to DHCP server port
    const sensor_err_t err = sensor_udp_bind(control_block, SENSOR_IP_ANY_TYPE, CONFIG::DHCP_SERVER_PORT);
    if (err != SENSOR_ERR_OK) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Failed to bind to port %d: %d\n", CONFIG::DHCP_SERVER_PORT, err);
#endif
        sensor_udp_remove(control_block);
        control_block = nullptr;
        return -1;
    }
    
#if SENSOR_DHCP_DEBUG
    printf("DHCP: Server listening on port %d\n", CONFIG::DHCP_SERVER_PORT);
    printf("DHCP: Server IP: %u.%u.%u.%u\n", 
           ip4_addr1(&server_ip), ip4_addr2(&server_ip), 
           ip4_addr3(&server_ip), ip4_addr4(&server_ip));
#endif
    
    return SENSOR_ERR_OK;
}

void DhcpServer::deinit() {
    if (control_block != nullptr) {
        sensor_udp_remove(control_block);
        control_block = nullptr;
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Server deinitialized\n");
#endif
    }
}

sensor_udp_recv_fn DhcpServer::udp_process_request_function = [](void* arg, udp_pcb_control_block* pcb,
                                                                 struct pbuf* packet,
                                                                 const sensor_ip_addr_t* sender_ip,
                                                                 sensor_u16_t sender_port) -> void
{
    [[maybe_unused]] auto* server = static_cast<DhcpServer*>(arg);
    
    if (packet->tot_len < DHCP_MIN_PACKET_SIZE) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Packet too small (%d bytes)\n", packet->tot_len);
#endif
        sensor_pbuf_free(packet);
        return;
    }
    
    dhcp_message_t request{};
    const std::size_t copied = sensor_pbuf_copy_partial(packet, &request, sizeof(request), 0);
    if (copied < DHCP_MIN_PACKET_SIZE) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Failed to copy packet data\n");
#endif
        sensor_pbuf_free(packet);
        return;
    }
    
    // Skip packets that are responses (we only handle requests)
    if (request.op != 1) { // BOOTREQUEST
        sensor_pbuf_free(packet);
        return;
    }
    
    // Find message type option
    std::uint8_t* options = request.options + 4; // Skip magic cookie
    std::uint8_t* msg_type_opt = server->find_dhcp_option(options, DhcpOption::MSG_TYPE);
    if (msg_type_opt == nullptr) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: No message type option found\n");
#endif
        sensor_pbuf_free(packet);
        return;
    }
    
    const auto message_type = static_cast<DhcpMessageType>(msg_type_opt[2]);
    
    switch (message_type) {
        case DhcpMessageType::DISCOVER:
            server->process_dhcp_discover(request, sender_ip, sender_port);
            break;
            
        case DhcpMessageType::REQUEST:
            server->process_dhcp_request(request, sender_ip, sender_port);
            break;
            
        default:
#if SENSOR_DHCP_DEBUG
            printf("DHCP: Unhandled message type: %d\n", static_cast<int>(message_type));
#endif
            break;
    }

    sensor_pbuf_free(packet);
};

std::uint8_t* DhcpServer::find_dhcp_option(std::uint8_t* options, DhcpOption option_type) {
    for (int i = 0; i < 308 && options[i] != static_cast<std::uint8_t>(DhcpOption::END);) {
        if (options[i] == static_cast<std::uint8_t>(option_type)) {
            return &options[i];
        }
        i += 2 + options[i + 1];
    }
    return nullptr;
}

void DhcpServer::write_dhcp_option_u8(std::uint8_t** opt, DhcpOption option, std::uint8_t value) {
    std::uint8_t* o = *opt;
    *o++ = static_cast<std::uint8_t>(option);
    *o++ = 1;
    *o++ = value;
    *opt = o;
}

void DhcpServer::write_dhcp_option_u32(std::uint8_t** opt, DhcpOption option, std::uint32_t value) {
    std::uint8_t* o = *opt;
    *o++ = static_cast<std::uint8_t>(option);
    *o++ = 4;
    *o++ = value >> 24;
    *o++ = value >> 16;
    *o++ = value >> 8;
    *o++ = value;
    *opt = o;
}

void DhcpServer::write_dhcp_option_bytes(std::uint8_t** opt, DhcpOption option, 
                                        std::size_t len, const void* data) {
    std::uint8_t* o = *opt;
    *o++ = static_cast<std::uint8_t>(option);
    *o++ = len;
    std::memcpy(o, data, len);
    *opt = o + len;
}

int DhcpServer::find_available_lease(const std::uint8_t* client_mac) {
    // First, check if MAC already has a lease
    for (std::size_t i = 0; i < CONFIG::MAX_DHCP_CLIENTS; ++i) {
        if (is_mac_equal(leases[i].mac.data(), client_mac)) {
            return static_cast<int>(i);
        }
    }
    
    // Look for free or expired lease
    for (std::size_t i = 0; i < CONFIG::MAX_DHCP_CLIENTS; ++i) {
        if (is_mac_empty(leases[i].mac.data()) || is_lease_expired(leases[i])) {
            // Reset the lease
            std::memcpy(leases[i].mac.data(), client_mac, CONFIG::MAC_ADDRESS_LEN);
            leases[i].is_active = true;
            return static_cast<int>(i);
        }
    }
    
    return -1; // No available leases
}

bool DhcpServer::is_mac_equal(const std::uint8_t* mac1, const std::uint8_t* mac2) {
    return std::memcmp(mac1, mac2, CONFIG::MAC_ADDRESS_LEN) == 0;
}

bool DhcpServer::is_mac_empty(const std::uint8_t* mac) {
    static const std::uint8_t empty_mac[CONFIG::MAC_ADDRESS_LEN] = {0};
    return std::memcmp(mac, empty_mac, CONFIG::MAC_ADDRESS_LEN) == 0;
}

bool DhcpServer::is_lease_expired(const dhcp_lease_t& lease) {
    const std::uint32_t current_time = sensor_cyw43_hal_ticks_ms() >> 16;
    return static_cast<std::int32_t>(lease.expiry - current_time) < 0;
}

void DhcpServer::process_dhcp_discover(const dhcp_message_t& request, 
                                      const sensor_ip_addr_t* client_addr, 
                                      std::uint16_t client_port) {
    const int lease_index = find_available_lease(request.chaddr);
    if (lease_index < 0) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: No available IP addresses\n");
#endif
        return;
    }
    
    const std::uint8_t client_ip_offset = base_ip_offset + lease_index;
    pbuf* response = create_dhcp_response(request, DhcpMessageType::OFFER, client_ip_offset);
    
    if (response != nullptr) {
        sensor_udp_sendto(control_block, response, client_addr, client_port);
        sensor_pbuf_free(response);
        
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Sent OFFER to %02x:%02x:%02x:%02x:%02x:%02x for IP %u.%u.%u.%u\n",
               request.chaddr[0], request.chaddr[1], request.chaddr[2], 
               request.chaddr[3], request.chaddr[4], request.chaddr[5],
               ip4_addr1(&server_ip), ip4_addr2(&server_ip), 
               ip4_addr3(&server_ip), client_ip_offset);
#endif
    }
}

void DhcpServer::process_dhcp_request(const dhcp_message_t& request, 
                                     const sensor_ip_addr_t* client_addr, 
                                     std::uint16_t client_port) {
    std::uint8_t* requested_ip_opt = find_dhcp_option(
        const_cast<std::uint8_t*>(request.options) + 4, DhcpOption::REQUESTED_IP);
    
    if (requested_ip_opt == nullptr) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: REQUEST without requested IP option\n");
#endif
        return;
    }
    
    // Verify requested IP is in our subnet
    if (std::memcmp(requested_ip_opt + 2, &server_ip, 3) != 0) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Requested IP not in our subnet\n");
#endif
        return;
    }
    
    const std::uint8_t requested_offset = requested_ip_opt[5];
    const std::uint8_t lease_index = requested_offset - base_ip_offset;
    
    if (lease_index >= CONFIG::MAX_DHCP_CLIENTS) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Invalid IP offset requested\n");
#endif
        return;
    }
    
    // Check if this MAC can use this IP
    if (!is_mac_equal(leases[lease_index].mac.data(), request.chaddr) && 
        !is_mac_empty(leases[lease_index].mac.data()) && 
        !is_lease_expired(leases[lease_index])) {
#if SENSOR_DHCP_DEBUG
        printf("DHCP: IP already in use by another client\n");
#endif
        return;
    }
    
    // Assign the lease
    std::memcpy(leases[lease_index].mac.data(), request.chaddr, CONFIG::MAC_ADDRESS_LEN);
    leases[lease_index].expiry = (sensor_cyw43_hal_ticks_ms() + CONFIG::DEFAULT_LEASE_TIME_S * 1000) >> 16;
    leases[lease_index].is_active = true;
    
    pbuf* response = create_dhcp_response(request, DhcpMessageType::ACK, requested_offset);
    
    if (response != nullptr) {
        sensor_udp_sendto(control_block, response, client_addr, client_port);
        sensor_pbuf_free(response);
        
#if SENSOR_DHCP_DEBUG
        printf("DHCP: Client connected - MAC: %02x:%02x:%02x:%02x:%02x:%02x IP: %u.%u.%u.%u\n",
               request.chaddr[0], request.chaddr[1], request.chaddr[2], 
               request.chaddr[3], request.chaddr[4], request.chaddr[5],
               ip4_addr1(&server_ip), ip4_addr2(&server_ip), 
               ip4_addr3(&server_ip), requested_offset);
#endif
    }
}

pbuf* DhcpServer::create_dhcp_response(const dhcp_message_t& request, 
                                      DhcpMessageType response_type, 
                                      std::uint8_t client_ip_offset) {
    const std::size_t response_size = sizeof(dhcp_message_t);
    pbuf* response = sensor_pbuf_alloc(SENSOR_PBUF_TRANSPORT, response_size, PBUF_RAM);
    if (response == nullptr) {
        return nullptr;
    }
    
    auto* resp_msg = static_cast<dhcp_message_t*>(response->payload);
    
    // Copy request and modify for response
    *resp_msg = request;
    resp_msg->op = DHCP_BOOTREPLY;
    resp_msg->yiaddr[0] = sensor_ip4_addr1(&server_ip);
    resp_msg->yiaddr[1] = sensor_ip4_addr2(&server_ip);
    resp_msg->yiaddr[2] = sensor_ip4_addr3(&server_ip);
    resp_msg->yiaddr[3] = client_ip_offset;
    
    // Set magic cookie
    std::uint8_t* opt = resp_msg->options;
    *opt++ = 0x63; *opt++ = 0x82; *opt++ = 0x53; *opt++ = 0x63;
    
    // Write DHCP options
    write_dhcp_option_u8(&opt, DhcpOption::MSG_TYPE, static_cast<std::uint8_t>(response_type));
    write_dhcp_option_bytes(&opt, DhcpOption::SERVER_ID, 4, &server_ip);
    write_dhcp_option_bytes(&opt, DhcpOption::SUBNET_MASK, 4, &netmask);
    write_dhcp_option_bytes(&opt, DhcpOption::ROUTER, 4, &server_ip);     // Gateway = us
    write_dhcp_option_bytes(&opt, DhcpOption::DNS, 4, &server_ip);        // DNS = us (THE MAGIC!)
    write_dhcp_option_u32(&opt, DhcpOption::IP_LEASE_TIME, CONFIG::DEFAULT_LEASE_TIME_S);
    *opt++ = static_cast<std::uint8_t>(DhcpOption::END);
    
    return response;
}