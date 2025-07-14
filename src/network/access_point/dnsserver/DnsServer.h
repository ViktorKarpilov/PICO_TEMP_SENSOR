//
// Created by PC on 7/12/2025.
//

#ifndef DNSSERVER_H
#define DNSSERVER_H

#define DNS_MAX_HOST_NAME_LEN 128

#include <lwip/udp.h>

#include "lwip/ip_addr.h"


class DnsServer {
public:
    explicit DnsServer();
    int init();
    
private:
    udp_pcb *control_block;
    static udp_recv_fn udp_process_request_function;
};



#endif //DNSSERVER_H
