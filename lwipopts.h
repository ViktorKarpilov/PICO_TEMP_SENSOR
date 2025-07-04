#ifndef LWIPOPTS_H
#define LWIPOPTS_H

// MINIMAL lwipopts.h for Pico 2 W Wi-Fi
// Just the bare essentials to compile without errors!

// Bare metal (no OS)
#define NO_SYS                      1

// Basic memory settings
#define MEM_ALIGNMENT               4
#define MEM_SIZE                    4000

// Essential protocols only
#define LWIP_ARP                    1
#define LWIP_ETHERNET               1
#define LWIP_ICMP                   1
#define LWIP_DHCP                   1
#define LWIP_UDP                    1
#define LWIP_TCP                    1

// Disable advanced features
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0
#define LWIP_STATS                  0
#define LWIP_IPV6                   0

#endif /* LWIPOPTS_H */