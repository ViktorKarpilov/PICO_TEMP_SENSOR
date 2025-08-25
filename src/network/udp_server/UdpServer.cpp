//
// Created by PC on 8/18/2025.
//

#include "UdpServer.h"

#include <cstring>

#include "src/config.h"
#include "src/src.h"

using namespace std;

void default_udp_receive_callback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port)
{
    const auto state = static_cast<UpdServerState*>(arg);

    state->mqtt_broker_addr = *addr;

    uint32_t addr_for_print = state->mqtt_broker_addr.value().addr;
    printf("Broker addr: %lu.%lu.%lu.%lu\n",
           (addr_for_print >> 0) & 0xFF,
           (addr_for_print >> 8) & 0xFF,
           (addr_for_print >> 16) & 0xFF,
           (addr_for_print >> 24) & 0xFF);
    
    if (state->listener_pcb)
    {
        udp_remove(state->listener_pcb);
        state->listener_pcb = nullptr;
    }

    if (p)
    {
        pbuf_free(p);
    }
}

UdpServer::UdpServer()
{
    state = new UpdServerState();

    state->listener_pcb = udp_new();

    ip_set_option(state->listener_pcb, SOF_BROADCAST);

    udp_bind(state->listener_pcb, IP_ADDR_ANY, CONFIG::BEACON_PORT);
    udp_recv(state->listener_pcb, default_udp_receive_callback, state);
}

UdpServer::~UdpServer()
{
    udp_remove(state->listener_pcb);

    delete state;
}

void UdpServer::get_mqtt_broker_ip() const
{
    if (this->state->mqtt_broker_addr)
    {
        return;
    }

    if (pbuf* request = pbuf_alloc(PBUF_TRANSPORT, strlen(CONFIG::MQTT_CALL_MESSAGE), PBUF_RAM))
    {
        memcpy(request->payload, CONFIG::MQTT_CALL_MESSAGE, strlen(CONFIG::MQTT_CALL_MESSAGE));
        request->len = strlen(CONFIG::MQTT_CALL_MESSAGE);
        request->tot_len = strlen(CONFIG::MQTT_CALL_MESSAGE);

        udp_sendto(this->state->listener_pcb, request, IP_ADDR_BROADCAST, CONFIG::BEACON_PORT);

        pbuf_free(request);
    }
}
