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

enum RequestType
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
    
    // Static callbacks for lwIP (these are called by the network stack)
    static err_t accept_callback(void* arg, tcp_pcb* newpcb, err_t err);
    static err_t recv_callback(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err);
    static err_t sent_callback(void* arg, tcp_pcb* tpcb, u16_t len);
    static err_t poll_callback(void* arg, tcp_pcb* tpcb);
    static void error_callback(void* arg, err_t err);
    
    // Helper methods for connection management
    static err_t send_response_data(tcp_pcb* tpcb, http_connection_state* conn_state);
    static void cleanup_connection(tcp_pcb* tpcb, http_connection_state* conn_state);
    
    // HTTP response builders
    static std::string build_captive_portal_response();
    static std::string build_connectivity_check_response(const std::string& request);
    static std::string build_freezer_config_page();
    static std::string build_status_api_response();

    static RequestType determine_request_type(const std::string& request);

    // Request parsers
    static bool is_connectivity_check(const std::string& request);
    static bool is_config_request(const std::string& request);
    static bool is_api_request(const std::string& request);
    static bool is_connection_response(const std::string& request);

    inline static std::pair<RequestChecker, RequestType> request_types_reference_table[] = {
        {is_connectivity_check, ConnectivityCheck},
        {is_config_request, ConfigRequest}, 
        {is_api_request, StatusRequest},
        {is_connection_response, ConnectionResponse}
    };
};

#endif //HTTPSERVER_H