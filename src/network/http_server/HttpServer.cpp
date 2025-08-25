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
#include "Helpers/HttpServerHelpers.h"
#include "pico/cyw43_arch.h"
#include "src/src.h"
#include "src/enviroment_sensor/enviroment_sensor.h"

#define HTTP_SERVER_DEBUG 0

#ifdef HTTP_SERVER_DEBUG
#define HTTP_SERVER_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define HTTP_SERVER_PRINT(fmt, ...) ((void)0)
#endif

using namespace std;

// Connection state structure for each client
struct http_connection_state
{
    std::string response_data;
    size_t bytes_sent;
    size_t bytes_queued;

    size_t bytes_received;
    bool response_ready;
    HTTPMessage* request;
    bool request_ready;

    http_connection_state() : bytes_sent(0), bytes_queued(0), bytes_received(0), response_ready(false),
                              request(nullptr), request_ready(false)
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

class HttpServer::HttpServerCommunication
{
public:
    static err_t accept_callback(void* arg, tcp_pcb* newpcb, err_t err)
    {
        if (err != ERR_OK || newpcb == nullptr)
        {
            HTTP_SERVER_PRINT("❌ HTTP: Accept error: %d\n", err);
            return ERR_VAL;
        }
        tcp_arg(newpcb, new http_connection_state());
        tcp_recv(newpcb, recv_callback);
        tcp_sent(newpcb, sent_callback);
        tcp_err(newpcb, error_callback);
        tcp_poll(newpcb, poll_callback, 4); // Poll every 2 seconds (4 * 0.5s)

        tcp_setprio(newpcb, TCP_PRIO_MIN);

        return ERR_OK;
    }

    static err_t recv_callback(void* arg, struct tcp_pcb* tpcb, pbuf* package, err_t err)
    {
        auto* conn_state = static_cast<http_connection_state*>(arg);

        if (err != ERR_OK)
        {
            HTTP_SERVER_PRINT("❌ HTTP: Receive error: %d\n", err);
            cleanup_connection(tpcb, conn_state);
            return ERR_ABRT;
        }

        if (package == nullptr)
        {
            HTTP_SERVER_PRINT("HTTP: Client closed connection\n");
            cleanup_connection(tpcb, conn_state);
            return ERR_OK;
        }

        if (package->tot_len > CONFIG::MAX_REQUEST_SIZE - 1)
        {
            return ERR_VAL;
        }
        if (conn_state->request == nullptr)
        {
            conn_state->request = new HTTPMessage();
        }

        char request_buffer[CONFIG::MAX_REQUEST_SIZE];
        pbuf_copy_partial(package, request_buffer,
                          package->tot_len, 0);

        HttpServerHelpers::parse_request_package(request_buffer, *conn_state->request);

        std::string response;
        switch (determine_request_type(conn_state->request->start_line))
        {
        case StatusRequest:
            response = HttpServerHelpers::build_status_api_response(EnvironmentSensor::readTemperature(),
                                                                    EnvironmentSensor::readHumidity());
            break;
        case ConfigRequest:
            response = HttpServerHelpers::build_freezer_config_page();
            HTTP_SERVER_PRINT("HTTP: Serving config page, length: %d\n", response.length());
            break;
        case ConnectionResponse:
            {
                string ssid;
                string pass;

                response = HttpServerHelpers::connection_request_handler(*conn_state->request, pass, ssid);
                HTTP_SERVER_PRINT("HTTP: Handling connection response pass:%s ssid:%s\n", pass.c_str(), ssid.c_str());
                if (!pass.empty() && !ssid.empty())
                {
                    get_wifi_service().schedule_connect_user_network(ssid, pass);
                }
                
                break;
            }
        case ConnectivityCheck:
            response = HttpServerHelpers::build_connectivity_check_response(conn_state->request->start_line);
            HTTP_SERVER_PRINT("HTTP: Connectivity check response\n");
            break;
        case GetSSIDs:
            {
                const auto ssids = get_wifi_service().get_ssids();
                response = HttpServerHelpers::build_get_ssids_response(ssids);
                HTTP_SERVER_PRINT("HTTP: response:%s \n", response.c_str());
                break;
            }
        case Unknown:
            response = HttpServerHelpers::build_captive_portal_response();
            HTTP_SERVER_PRINT("HTTP: Redirecting to captive portal\n");
            break;
        }

        // Store response in connection state
        conn_state->response_data = response;
        conn_state->response_ready = true;
        conn_state->bytes_sent = 0;
        conn_state->bytes_queued = 0;

        // Tell lwIP we've processed the received data
        tcp_recved(tpcb, package->tot_len);
        pbuf_free(package);

        // Try to send the response immediately
        err_t write_err = send_response_data(tpcb, conn_state);
        if (write_err != ERR_OK && write_err != ERR_MEM)
        {
            // ERR_MEM is OK - just means TCP buffer is full, will retry later
            HTTP_SERVER_PRINT("❌ HTTP: Failed to send response: %d\n", write_err);
            cleanup_connection(tpcb, conn_state);
            return ERR_ABRT;
        }

        return ERR_OK;
    }

    static err_t sent_callback(void* arg, tcp_pcb* tpcb, u16_t len)
    {
        auto* conn_state = static_cast<http_connection_state*>(arg);

        conn_state->bytes_sent += len;

        if (conn_state->bytes_sent >= conn_state->bytes_queued &&
            conn_state->bytes_queued >= conn_state->response_data.length())
        {
            cleanup_connection(tpcb, conn_state);
        }
        else if (conn_state->bytes_queued < conn_state->response_data.length())
        {
            err_t write_err = send_response_data(tpcb, conn_state);
            if (write_err != ERR_OK && write_err != ERR_MEM)
            {
                HTTP_SERVER_PRINT("❌ HTTP: Failed to queue remaining data: %d\n", write_err);
                cleanup_connection(tpcb, conn_state);
            }
        }

        return ERR_OK;
    }

    static err_t poll_callback(void* arg, tcp_pcb* tpcb)
    {
        HTTP_SERVER_PRINT("HTTP: Connection timeout, cleaning up\n");
        auto* conn_state = static_cast<http_connection_state*>(arg);
        cleanup_connection(tpcb, conn_state);
        return ERR_ABRT;
    }

    static void error_callback(void* arg, err_t err)
    {
        HTTP_SERVER_PRINT("❌ HTTP: Connection error: %d\n", err);
        // Connection already closed by lwIP, just clean up our state
        auto* conn_state = static_cast<http_connection_state*>(arg);
        delete conn_state;
    }

    static err_t send_response_data(tcp_pcb* tpcb, http_connection_state* conn_state)
    {
        if (!conn_state->response_ready || conn_state->bytes_queued >= conn_state->response_data.length())
        {
            return ERR_OK;
        }

        size_t remaining = conn_state->response_data.length() - conn_state->bytes_queued;
        size_t tcp_send_buffer_space = tcp_sndbuf(tpcb);

        if (tcp_send_buffer_space == 0)
        {
            HTTP_SERVER_PRINT("TCP: Send buffer full, waiting...\n");
            return ERR_OK;
        }

        size_t to_send = std::min(remaining, tcp_send_buffer_space);

        const char* data_ptr = conn_state->response_data.c_str() + conn_state->bytes_queued;

        // Send the data
        err_t err = tcp_write(tpcb, data_ptr, to_send, TCP_WRITE_FLAG_COPY);

        if (err == ERR_OK)
        {
            conn_state->bytes_queued += to_send;
            err_t output_err = tcp_output(tpcb);

            if (output_err != ERR_OK)
            {
                HTTP_SERVER_PRINT("❌ TCP output failed: %d\n", output_err);
                return output_err;
            }
        }
        else
        {
            HTTP_SERVER_PRINT("❌ TCP write failed: %d\n", err);
        }

        return err;
    }

    static void cleanup_connection(tcp_pcb* tpcb, http_connection_state* conn_state)
    {
        delete conn_state;

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

#pragma region RequestParsers

    static RequestType determine_request_type(const std::string& request)
    {
        for (const auto& [type_check_function, request_type] : request_types_reference_table)
        {
            if (type_check_function(request)) return request_type;
        }
        return Unknown;
    }

    static bool is_android_internet_check(const std::string& request)
    {
        return request.find("connectivitycheck.gstatic.com") != std::string::npos ||
            request.find("generate_204") != std::string::npos ||
            request.find("readaloud.googleapis.com") != std::string::npos ||
            request.find("clients4.google.com") != std::string::npos ||
            request.find("alt3-mtalk.google.com") != std::string::npos;
    }

    // Response building methods (unchanged from your original)
    static bool is_connectivity_check(const std::string& request)
    {
        return request.find("msftconnecttest.com") != std::string::npos ||
            request.find("captive.apple.com") != std::string::npos ||
            request.find("redirect") != std::string::npos ||
            is_android_internet_check(request);
    }


    static bool is_config_request(const std::string& request)
    {
        return request.find("GET /config") != std::string::npos ||
            request.find("GET /setup") != std::string::npos ||
            request.find("POST /config") != std::string::npos;
    }

    static bool is_api_request(const std::string& request)
    {
        return request.find("GET /api/status") != std::string::npos;
    }

    static bool is_connection_response(const std::string& request)
    {
        return request.find("POST /api/connection") != std::string::npos;
    }

    static bool is_get_ssids(const std::string& request)
    {
        return request.find("GET /api/ssids") != std::string::npos;
    }

#pragma endregion RequestParsers

    inline static std::pair<RequestChecker, RequestType> request_types_reference_table[] = {
        {is_connectivity_check, ConnectivityCheck},
        {is_config_request, ConfigRequest},
        {is_api_request, StatusRequest},
        {is_connection_response, ConnectionResponse},
        {is_get_ssids, GetSSIDs}
    };
};

int HttpServer::init(uint16_t port)
{
    server_pcb = tcp_new();
    if (server_pcb == nullptr)
    {
        HTTP_SERVER_PRINT("❌ HTTP: Failed to create TCP PCB\n");
        return -1;
    }

    // Bind to any interface
    err_t err = tcp_bind(server_pcb, IP_ADDR_ANY, port);
    if (err != ERR_OK)
    {
        HTTP_SERVER_PRINT("❌ HTTP: Failed to bind to port %d - Error: %d\n", port, err);
        tcp_close(server_pcb);
        server_pcb = nullptr;
        return -1;
    }

    // Start listening
    server_pcb = tcp_listen(server_pcb);
    if (server_pcb == nullptr)
    {
        HTTP_SERVER_PRINT("❌ HTTP: Failed to listen\n");
        return -1;
    }

    // Set server callback
    tcp_arg(server_pcb, this);
    tcp_accept(server_pcb, HttpServerCommunication::accept_callback);

    return 0;
}

void HttpServer::deinit()
{
    if (server_pcb != nullptr)
    {
        tcp_close(server_pcb);
        server_pcb = nullptr;
        HTTP_SERVER_PRINT("HTTP: Server deinitialized\n");
    }
}

bool HttpServer::test_can_bind()
{
    HTTP_SERVER_PRINT("Testing if we can bind to port 80...\n");

    tcp_pcb* test_pcb = tcp_new();
    if (!test_pcb)
    {
        HTTP_SERVER_PRINT("❌ Cannot create TCP PCB\n");
        return false;
    }

    err_t err = tcp_bind(test_pcb, IP_ADDR_ANY, 80);
    tcp_close(test_pcb);

    if (err != ERR_OK)
    {
        HTTP_SERVER_PRINT("❌ Cannot bind to port 80, error code: %d\n", err);
        return false;
    }

    HTTP_SERVER_PRINT("✅ Can bind to port 80\n");
    return true;
}
