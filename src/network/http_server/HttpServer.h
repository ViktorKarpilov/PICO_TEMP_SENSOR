//
// Created by PC on 7/14/2025.
//
//
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <lwip/tcp.h>
#include <string>
#include <src/network/wifi_service/WifiService.h>

#define LWIP_DEBUG 1
#define TCP_DEBUG LWIP_DBG_ON
#define HTTPD_DEBUG LWIP_DBG_ON

// Forward declaration for connection state
struct http_connection_state;

enum RequestType : uint8_t
{
    StatusRequest,
    ConfigRequest,

    ConnectionResponse,

    ConnectivityCheck,
    Unknown
};

typedef bool (*RequestChecker)(const std::string&);

class HttpServer {
public:
    HttpServer(WifiService *wifi_service);
    ~HttpServer();
    
    int init(uint16_t port = 80);
    void deinit();
    static bool test_can_bind();
    
private:
    tcp_pcb* server_pcb;
    WifiService *wifi_service;

    class HttpServerCommunication;
};

#endif //HTTPSERVER_H