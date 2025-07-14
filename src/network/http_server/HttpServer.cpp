//
// Created by PC on 7/14/2025.
//

#include "HttpServer.h"
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "lwip/tcp.h"
#include "lwip/pbuf.h"

// Include the generated HTML headers
#include <src/config.h>

#include "generated/config_html.h"

constexpr uint16_t HTTP_PORT = 80;
constexpr size_t MAX_REQUEST_SIZE = 1024;

HttpServer::HttpServer() : server_pcb(nullptr) {
}

HttpServer::~HttpServer() {
    deinit();
}

int HttpServer::init(uint16_t port) {
    printf("🌐 HTTP: Starting server initialization...\n");
    printf("   Port: %d\n", port);
    
    server_pcb = tcp_new();
    if (server_pcb == nullptr) {
        printf("❌ HTTP: Failed to create TCP PCB\n");
        return -1;
    }
    printf("✅ HTTP: TCP PCB created\n");
    
    // 🎯 BIND TO ANY INTERFACE INSTEAD OF SPECIFIC IP
    err_t err = tcp_bind(server_pcb, IP_ADDR_ANY, port);
    if (err != ERR_OK) {
        printf("❌ HTTP: Failed to bind to ANY:%d - Error: %d\n", port, err);
        tcp_close(server_pcb);
        server_pcb = nullptr;
        return -1;
    }
    printf("✅ HTTP: Successfully bound to ANY:%d (all interfaces)\n", port);
    
    server_pcb = tcp_listen(server_pcb);
    if (server_pcb == nullptr) {
        printf("❌ HTTP: Failed to listen\n");
        return -1;
    }
    printf("✅ HTTP: Listening for connections\n");
    
    tcp_arg(server_pcb, this);
    tcp_accept(server_pcb, accept_callback);
    
    printf("🎉 HTTP: Server listening on ANY:%d\n", port);
    return 0;
}

void HttpServer::deinit() {
    if (server_pcb != nullptr) {
        tcp_close(server_pcb);
        server_pcb = nullptr;
    }
}

err_t HttpServer::accept_callback(void* arg, tcp_pcb* newpcb, err_t err) {
    if (err != ERR_OK || newpcb == nullptr) {
        return ERR_VAL;
    }
    
    printf("HTTP: New connection from %s\n", ip4addr_ntoa(ip_2_ip4(&newpcb->remote_ip)));
    
    tcp_arg(newpcb, arg);
    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_err(newpcb, error_callback);
    
    return ERR_OK;
}

err_t HttpServer::recv_callback(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err) {
    if (p == nullptr) {
        // Connection closed
        tcp_close(tpcb);
        return ERR_OK;
    }
    
    // Copy request data - 🎯 FIXED TYPE MISMATCH!
    char request_buffer[MAX_REQUEST_SIZE];
    size_t request_len = pbuf_copy_partial(p, request_buffer, 
                                          std::min(static_cast<size_t>(p->tot_len), MAX_REQUEST_SIZE - 1), 0);
    request_buffer[request_len] = '\0';
    
    std::string request(request_buffer);
    std::string response;
    
    printf("HTTP: Request: %.100s...\n", request.c_str());
    
    // Handle different types of requests
    if (is_api_request(request)) {
        // API endpoint for real-time status
        response = build_status_api_response();
        printf("HTTP: Serving API request\n");
    } else if (is_connectivity_check(request)) {
        // Respond to connectivity checks to trigger captive portal
        response = build_connectivity_check_response();
        printf("HTTP: Responding to connectivity check\n");
    } else if (is_config_request(request)) {
        // Serve the actual configuration page
        response = build_freezer_config_page();
        printf("HTTP: Serving config page\n");
    } else {
        // Default: redirect to captive portal
        response = build_captive_portal_response();
        printf("HTTP: Redirecting to captive portal\n");
    }
    
    send_http_response(tpcb, response);
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    
    return ERR_OK;
}

err_t HttpServer::sent_callback(void* arg, tcp_pcb* tpcb, u16_t len) {
    // Close connection after sending response
    tcp_close(tpcb);
    return ERR_OK;
}

void HttpServer::error_callback(void* arg, err_t err) {
    printf("HTTP: Connection error: %d\n", err);
}

void HttpServer::send_http_response(tcp_pcb* tpcb, const std::string& response) {
    tcp_write(tpcb, response.c_str(), response.length(), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
}

bool HttpServer::is_connectivity_check(const std::string& request) {
    // Check for common connectivity check URLs
    return request.find("msftconnecttest.com") != std::string::npos ||
           request.find("connectivitycheck.gstatic.com") != std::string::npos ||
           request.find("captive.apple.com") != std::string::npos ||
           request.find("generate_204") != std::string::npos ||
           request.find("redirect") != std::string::npos;
}

bool HttpServer::is_config_request(const std::string& request) {
    // Check if this is a request for the config page
    return request.find("GET /config") != std::string::npos ||
           request.find("GET /setup") != std::string::npos ||
           request.find("POST /config") != std::string::npos;
}

bool HttpServer::is_api_request(const std::string& request) {
    // Check if this is an API request
    return request.find("GET /api/status") != std::string::npos;
}

std::string HttpServer::build_connectivity_check_response() {
    // Return HTTP 302 redirect to trigger captive portal detection
    return "HTTP/1.1 302 Found\r\n"
           "Location: http://7.7.7.7/config\r\n"
           "Content-Length: 0\r\n"
           "Connection: close\r\n"
           "\r\n";
}

std::string HttpServer::build_captive_portal_response() {
    // Redirect any other request to config page
    return "HTTP/1.1 302 Found\r\n"
           "Location: http://7.7.7.7/config\r\n"
           "Content-Length: 0\r\n"
           "Connection: close\r\n"
           "\r\n";
}

std::string HttpServer::build_freezer_config_page() {
    // 🎯 USE THE GENERATED HTML!
    const std::string& html = HtmlResources::CONFIG_PAGE;
    
    std::string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html; charset=utf-8\r\n"
                          "Content-Length: " + std::to_string(html.length()) + "\r\n"
                          "Cache-Control: no-cache\r\n"
                          "Connection: close\r\n"
                          "\r\n" + html;
    
    return response;
}

std::string HttpServer::build_status_api_response() {
    // Mock sensor readings for now - replace with real sensor calls
    float temperature = -18.5f;  // Replace with: EnvironmentSensor::readTemperature();
    float humidity = 45.2f;      // Replace with: EnvironmentSensor::readHumidity();
    
    // Build JSON response
    std::string json = "{"
                      "\"temperature\":" + std::to_string(temperature) + ","
                      "\"humidity\":" + std::to_string(humidity) + ","
                      "\"status\":\"active\""
                      "}";
    
    std::string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: " + std::to_string(json.length()) + "\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Connection: close\r\n"
                          "\r\n" + json;
    
    return response;
}