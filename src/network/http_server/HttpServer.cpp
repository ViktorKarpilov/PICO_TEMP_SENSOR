//
// Created by PC on 7/14/2025.
//

#include "HttpServer.h"
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <src/config.h>
#include <src/enviroment_sensor/enviroment_sensor.h>

#include "generated/config_html.h"

constexpr size_t MAX_REQUEST_SIZE = 8192;

// Connection state structure for each client
struct http_connection_state
{
    std::string response_data;
    size_t bytes_sent;
    bool response_ready;

    http_connection_state() : bytes_sent(0), response_ready(false)
    {
    }
};

HttpServer::HttpServer() : server_pcb(nullptr)
{
}

HttpServer::~HttpServer()
{
    deinit();
}

int HttpServer::init(uint16_t port)
{
    printf("🌐 HTTP: Starting server on port %d\n", port);

    server_pcb = tcp_new();
    if (server_pcb == nullptr)
    {
        printf("❌ HTTP: Failed to create TCP PCB\n");
        return -1;
    }

    // Bind to any interface
    err_t err = tcp_bind(server_pcb, IP_ADDR_ANY, port);
    if (err != ERR_OK)
    {
        printf("❌ HTTP: Failed to bind to port %d - Error: %d\n", port, err);
        tcp_close(server_pcb);
        server_pcb = nullptr;
        return -1;
    }

    // Start listening
    server_pcb = tcp_listen(server_pcb);
    if (server_pcb == nullptr)
    {
        printf("❌ HTTP: Failed to listen\n");
        return -1;
    }

    // Set server callback
    tcp_arg(server_pcb, this);
    tcp_accept(server_pcb, accept_callback);

    printf("🎉 HTTP: Server successfully listening on port %d\n", port);
    printf("🔧 HTTP Debug: PCB = %p\n", server_pcb);
    printf("🔧 HTTP PCB State: %d (should be %d for LISTEN)\n",
           server_pcb->state, LISTEN);
    return 0;
}

void HttpServer::deinit()
{
    if (server_pcb != nullptr)
    {
        tcp_close(server_pcb);
        server_pcb = nullptr;
        printf("HTTP: Server deinitialized\n");
    }
}

err_t HttpServer::accept_callback(void* arg, tcp_pcb* newpcb, err_t err)
{
    if (err != ERR_OK || newpcb == nullptr)
    {
        printf("❌ HTTP: Accept error: %d\n", err);
        return ERR_VAL;
    }

    printf("✅ HTTP: New connection from %s:%d\n",
           ip4addr_ntoa(ip_2_ip4(&newpcb->remote_ip)), newpcb->remote_port);

    // Create connection state for this client
    auto* conn_state = new http_connection_state();

    // Set up client connection callbacks
    tcp_arg(newpcb, conn_state);
    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_err(newpcb, error_callback);
    tcp_poll(newpcb, poll_callback, 4); // Poll every 2 seconds (4 * 0.5s)

    // Set reasonable timeouts
    tcp_setprio(newpcb, TCP_PRIO_MIN);

    return ERR_OK;
}

err_t HttpServer::recv_callback(void* arg, tcp_pcb* tpcb, pbuf* p, err_t err)
{
    auto* conn_state = static_cast<http_connection_state*>(arg);
    
    if (err != ERR_OK)
    {
        printf("❌ HTTP: Receive error: %d\n", err);
        cleanup_connection(tpcb, conn_state);
        return ERR_ABRT;
    }

    if (p == nullptr)
    {
        printf("HTTP: Client closed connection\n");
        cleanup_connection(tpcb, conn_state);
        return ERR_OK;
    }

    // Copy the HTTP request (same as before)
    char request_buffer[MAX_REQUEST_SIZE];
    size_t request_len = pbuf_copy_partial(p, request_buffer,
                                           std::min(static_cast<size_t>(p->tot_len), MAX_REQUEST_SIZE - 1), 0);
    request_buffer[request_len] = '\0';
    std::string request(request_buffer);

    printf("HTTP: Request received (%.100s...)\n", request.c_str());

    // Build response (same logic as before)
    std::string response;
    if (is_api_request(request))
    {
        response = build_status_api_response();
        printf("HTTP: Serving API request\n");
    }
    else if (is_connectivity_check(request))
    {
        response = build_connectivity_check_response(request);
        printf("HTTP: Connectivity check response\n");
    }
    else if (is_config_request(request))
    {
        response = build_freezer_config_page();
        printf("HTTP: Serving config page\n");
    }
    else
    {
        response = build_captive_portal_response();
        printf("HTTP: Redirecting to captive portal\n");
    }

    // Store response in connection state
    conn_state->response_data = response;
    conn_state->response_ready = true;
    conn_state->bytes_sent = 0; // RESET to 0 for new response

    // Tell lwIP we've processed the received data
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    // Try to send the response immediately
    err_t write_err = send_response_data(tpcb, conn_state);
    if (write_err != ERR_OK && write_err != ERR_MEM)
    {
        // ERR_MEM is OK - just means TCP buffer is full, will retry later
        printf("❌ HTTP: Failed to send response: %d\n", write_err);
        cleanup_connection(tpcb, conn_state);
        return ERR_ABRT;
    }

    return ERR_OK;
}

err_t HttpServer::sent_callback(void* arg, tcp_pcb* tpcb, u16_t len)
{
    auto* conn_state = static_cast<http_connection_state*>(arg);
    
    // This is the ONLY place where we increment bytes_sent
    // because 'len' tells us exactly how many bytes TCP actually sent
    conn_state->bytes_sent += len;
    
    printf("HTTP: TCP confirmed sent %d bytes, total: %zu/%zu\n",
           len, conn_state->bytes_sent, conn_state->response_data.length());

    if (conn_state->bytes_sent >= conn_state->response_data.length())
    {
        // All data sent, close connection
        printf("HTTP: Response complete, closing connection\n");
        cleanup_connection(tpcb, conn_state);
    }
    else
    {
        // Send remaining data
        printf("HTTP: Sending next chunk...\n");
        err_t write_err = send_response_data(tpcb, conn_state);
        if (write_err != ERR_OK)
        {
            printf("❌ HTTP: Failed to send remaining data: %d\n", write_err);
            cleanup_connection(tpcb, conn_state);
        }
    }
    
    return ERR_OK;
}

err_t HttpServer::poll_callback(void* arg, tcp_pcb* tpcb)
{
    // Connection timeout - clean up
    printf("HTTP: Connection timeout, cleaning up\n");
    auto* conn_state = static_cast<http_connection_state*>(arg);
    cleanup_connection(tpcb, conn_state);
    return ERR_ABRT;
}

void HttpServer::error_callback(void* arg, err_t err)
{
    printf("❌ HTTP: Connection error: %d\n", err);
    // Connection already closed by lwIP, just clean up our state
    auto* conn_state = static_cast<http_connection_state*>(arg);
    if (conn_state != nullptr)
    {
        delete conn_state;
    }
}

err_t HttpServer::send_response_data(tcp_pcb* tpcb, http_connection_state* conn_state)
{
    if (!conn_state->response_ready || conn_state->bytes_sent >= conn_state->response_data.length())
    {
        return ERR_OK;
    }

    if (conn_state->response_data.length() < 4096) { // Small response threshold
        // Send everything at once
        err_t err = tcp_write(tpcb, conn_state->response_data.c_str(), 
                             conn_state->response_data.length(), TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(tpcb);
            conn_state->bytes_sent = conn_state->response_data.length(); // Mark as complete
        }
        return err;
    }

    // Calculate how much data to send
    size_t remaining = conn_state->response_data.length() - conn_state->bytes_sent;
    size_t tcp_send_buffer_space = static_cast<size_t>(tcp_sndbuf(tpcb));
    
    if (tcp_send_buffer_space == 0)
    {
        printf("TCP: Send buffer full, waiting...\n");
        return ERR_OK; // Send buffer full, wait for sent callback
    }

    // Take minimum of what we want to send and what TCP can accept
    size_t to_send = std::min(remaining, tcp_send_buffer_space);
    
    // Get pointer to the data we haven't sent yet
    const char* data_ptr = conn_state->response_data.c_str() + conn_state->bytes_sent;
    
    printf("=== CHUNK DEBUG ===\n");
    printf("Attempting to send: bytes %zu-%zu (%zu bytes)\n", 
           conn_state->bytes_sent, conn_state->bytes_sent + to_send, to_send);
    printf("TCP buffer space: %zu bytes\n", tcp_send_buffer_space);
    printf("Chunk content: %.50s...\n", data_ptr);
    printf("==================\n");

    // Send the data - CRITICAL: tcp_write might queue less than requested!
    err_t err = tcp_write(tpcb, data_ptr, to_send, TCP_WRITE_FLAG_COPY);
    
    if (err == ERR_OK)
    {
        // Force TCP to actually send the data
        err_t output_err = tcp_output(tpcb);
        if (output_err != ERR_OK)
        {
            printf("❌ TCP output failed: %d\n", output_err);
            return output_err;
        }
        
        // IMPORTANT: Do NOT increment bytes_sent here!
        // TCP might not have sent all the data yet.
        // Only increment in sent_callback() when we know exactly how much was sent.
        
        printf("✅ TCP write queued successfully\n");
    }
    else
    {
        printf("❌ TCP write failed: %d\n", err);
    }
    
    return err;
}

void HttpServer::cleanup_connection(tcp_pcb* tpcb, http_connection_state* conn_state)
{
    if (conn_state != nullptr)
    {
        delete conn_state;
    }

    if (tpcb != nullptr)
    {
        tcp_arg(tpcb, nullptr);
        tcp_recv(tpcb, nullptr);
        tcp_sent(tpcb, nullptr);
        tcp_err(tpcb, nullptr);
        tcp_poll(tpcb, nullptr, 0);
        tcp_close(tpcb);
    }
}

bool is_android_internet_check(const std::string& request)
{
    return request.find("connectivitycheck.gstatic.com") != std::string::npos ||
        request.find("generate_204") != std::string::npos ||
        request.find("readaloud.googleapis.com") != std::string::npos ||
        request.find("clients4.google.com") != std::string::npos ||
        request.find("alt3-mtalk.google.com") != std::string::npos;
}

// Response building methods (unchanged from your original)
bool HttpServer::is_connectivity_check(const std::string& request)
{
    return request.find("msftconnecttest.com") != std::string::npos ||
        request.find("captive.apple.com") != std::string::npos ||
        request.find("redirect") != std::string::npos ||
            is_android_internet_check(request);
}


bool HttpServer::is_config_request(const std::string& request)
{
    return request.find("GET /config") != std::string::npos ||
        request.find("GET /setup") != std::string::npos ||
        request.find("POST /config") != std::string::npos;
}

bool HttpServer::is_api_request(const std::string& request)
{
    return request.find("GET /api/status") != std::string::npos;
}

std::string HttpServer::build_connectivity_check_response(const std::string& request)
{
    if (is_android_internet_check(request))
    {
        // Android expects 204 No Content for "internet is working"
        return "HTTP/1.1 204 No Content\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    return build_captive_portal_response();
}

std::string HttpServer::build_captive_portal_response()
{
    return "HTTP/1.1 302 Found\r\n"
        "Location: http://7.7.7.7/config\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";
}

std::string HttpServer::build_freezer_config_page()
{
    const std::string& html = HtmlResources::CONFIG_PAGE;
    
    // CRITICAL: Ensure exact byte count
    size_t actual_length = html.length();
    
    return "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(actual_length) + "\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n"
        "\r\n" + html;
}

std::string HttpServer::build_status_api_response()
{
    double temperature = EnvironmentSensor::readTemperature();
    double humidity = EnvironmentSensor::readHumidity();

    std::string json = "{"
        "\"temperature\":" + std::to_string(temperature) + ","
        "\"humidity\":" + std::to_string(humidity) + ","
        "\"status\":\"active\""
        "}";

    return "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(json.length()) + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n" + json;
}

bool HttpServer::test_can_bind()
{
    printf("Testing if we can bind to port 80...\n");

    tcp_pcb* test_pcb = tcp_new();
    if (!test_pcb)
    {
        printf("❌ Cannot create TCP PCB\n");
        return false;
    }

    err_t err = tcp_bind(test_pcb, IP_ADDR_ANY, 80);
    tcp_close(test_pcb);

    if (err != ERR_OK)
    {
        printf("❌ Cannot bind to port 80, error code: %d\n", err);
        return false;
    }

    printf("✅ Can bind to port 80\n");
    return true;
}
