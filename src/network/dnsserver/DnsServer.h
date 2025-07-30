//
// Created by PC on 7/12/2025.
//

#ifndef DNSSERVER_H
#define DNSSERVER_H

#define DNS_MAX_HOST_NAME_LEN 128
#include <hardware/network/network.h>


class DnsServer {
public:
    explicit DnsServer();
    ~DnsServer();

    void deinit();
    int init();
    
private:
    udp_pcb_control_block *control_block;
    static sensor_udp_recv_fn udp_process_request_function;
};



#endif //DNSSERVER_H
