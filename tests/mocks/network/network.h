//
// Created by PC on 7/26/2025.
//

#ifndef NETWORK_H
#define NETWORK_H

// Mock versions of the macros
#define sensor_ip4_addr1(ipaddr) ((uint8_t)(((ipaddr)->addr) & 0xff))
#define sensor_ip4_addr2(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 8) & 0xff))
#define sensor_ip4_addr3(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 16) & 0xff))
#define sensor_ip4_addr4(ipaddr) ((uint8_t)((((ipaddr)->addr) >> 24) & 0xff))

#endif //NETWORK_H
