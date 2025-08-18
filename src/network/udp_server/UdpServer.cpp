//
// Created by PC on 8/18/2025.
//

#include "UdpServer.h"

#include <cstring>

#include "src/config.h"

void default_udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    const auto state = static_cast<UpdServerState*>(arg);
    state->mqtt_broker_addr = addr;

    delete state->listener_pcb;
    state->listener_pcb = nullptr;
}

UdpServer::UdpServer()
{
    state = new UpdServerState();

    state->broadcast_pcb = udp_new();
    state->listener_pcb = udp_new();
    
    ip_set_option(state->broadcast_pcb, SOF_BROADCAST);

    udp_bind(state->listener_pcb, IP_ADDR_ANY, 10);
}

UdpServer::~UdpServer()
{
    udp_remove(state->broadcast_pcb);
    udp_remove(state->listener_pcb);

    delete state;
}

void UdpServer::get_mqtt_broker_ip() const
{
    if (this->state->mqtt_broker_addr != nullptr)
    {
        return;
    }

    pbuf* request = pbuf_alloc(PBUF_TRANSPORT, strlen(CONFIG::MQTT_CALL_MESSAGE), PBUF_RAM);

    udp_recv(state->listener_pcb, default_udp_receive_callback, state);

    if (request != nullptr) {
        memcpy(request->payload, CONFIG::MQTT_CALL_MESSAGE, strlen(CONFIG::MQTT_CALL_MESSAGE));
        request->len = strlen(CONFIG::MQTT_CALL_MESSAGE);
        request->tot_len = strlen(CONFIG::MQTT_CALL_MESSAGE);
    }
    
    udp_sendto(this->state->broadcast_pcb, request, IP_ADDR_BROADCAST, 10);
}


