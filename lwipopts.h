#ifndef LWIPOPTS_H
#define LWIPOPTS_H

// MINIMAL lwipopts.h for Pico 2 W Wi-Fi
// Just the bare essentials to compile without errors!

// Bare metal (no OS)
#define NO_SYS                      1

#define MEM_ALIGNMENT               4
#define MEM_SIZE                    16384
#define MEMP_NUM_PBUF               16
#define PBUF_POOL_SIZE              24

#define LWIP_ARP                    1
#define LWIP_ETHERNET               1
#define LWIP_ICMP                   1
#define LWIP_DHCP                   1
#define LWIP_UDP                    1
#define LWIP_TCP                    1
#define LWIP_MQTT                   1

// Disable advanced features
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0
#define LWIP_STATS                  0
#define LWIP_IPV6                   0

#define MQTT_OUTPUT_RINGBUF_SIZE    2048   // Increase from 1024
#define MQTT_VAR_HEADER_BUFFER_LEN  128    // Add this

// TCP settings for MQTT
#define TCP_MSS                     1460
#define TCP_WND                     (8 * TCP_MSS)
#define TCP_SND_BUF                 (8 * TCP_MSS)
#define TCP_SND_QUEUELEN            16
#define MEMP_NUM_TCP_SEG            32

#define LWIP_DEBUG                  0
#define MQTT_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_OFF
#define MEMP_DEBUG                  LWIP_DBG_OFF

#define MEMP_NUM_TCP_PCB            10  // Number of TCP connections
#define MEMP_NUM_TCP_PCB_LISTEN     5   // Number of listening TCP connections
#define MEMP_NUM_SYS_TIMEOUT        10  // Number of simultaneous timeouts

// Increase these for MQTT:
#define MQTT_REQ_MAX_IN_FLIGHT      5   // Max pending MQTT requests
#define MQTT_OUTPUT_RINGBUF_SIZE    4096 // Increase further if needed
#define MQTT_VAR_HEADER_BUFFER_LEN  256  // Increase from 128

#endif /* LWIPOPTS_H */
