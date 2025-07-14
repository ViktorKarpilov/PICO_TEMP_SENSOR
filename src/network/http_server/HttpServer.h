//
// Created by PC on 7/14/2025.
//
//
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <lwip/tcp.h>
#include <string>

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    
    int init(uint16_t port = 80);
    void deinit();
    
private:
    tcp_pcb* server_pcb;
    
    // Static callbacks for lwIP
    static err_t accept_callback(void* arg, tcp_pcb* newpcb, err_t err);
    static err_t recv_callback(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err);
    static err_t sent_callback(void* arg, tcp_pcb* tpcb, u16_t len);
    static void error_callback(void* arg, err_t err);
    
    // HTTP response builders
    static void send_http_response(tcp_pcb* tpcb, const std::string& response);
    static std::string build_captive_portal_response();
    static std::string build_connectivity_check_response();
    static std::string build_freezer_config_page();
    static std::string build_status_api_response();  // 🎯 ADDED THIS!
    
    // Request parser
    static bool is_connectivity_check(const std::string& request);
    static bool is_config_request(const std::string& request);
    static bool is_api_request(const std::string& request);  // 🎯 ADDED THIS!
};

#endif //HTTPSERVER_H