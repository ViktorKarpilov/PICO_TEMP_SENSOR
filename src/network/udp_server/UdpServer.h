//
// Created by PC on 8/18/2025.
//

#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <optional>

#include "lwip/ip_addr.h"
#include "lwip/udp.h"

struct UpdServerState
{
    // udp_pcb* broadcast_pcb{nullptr};
    udp_pcb* listener_pcb{nullptr};
    std::optional<ip_addr_t> mqtt_broker_addr{};
};


class UdpServer
{
public:
    UpdServerState* state;

    UdpServer();
    ~UdpServer();

    void get_mqtt_broker_ip() const;
};


#endif //UDPSERVER_H
