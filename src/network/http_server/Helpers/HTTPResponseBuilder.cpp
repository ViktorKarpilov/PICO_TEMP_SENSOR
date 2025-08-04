//
// Created by PC on 8/4/2025.
//

#include <string>
#include <generated/config_html.h>
#include "HttpServerHelpers.h"

namespace HttpServerHelpers
{
    bool is_android_internet_check(const std::string& request)
    {
        return request.find("connectivitycheck.gstatic.com") != std::string::npos ||
            request.find("generate_204") != std::string::npos ||
            request.find("readaloud.googleapis.com") != std::string::npos ||
            request.find("clients4.google.com") != std::string::npos ||
            request.find("alt3-mtalk.google.com") != std::string::npos;
    }

    std::string build_captive_portal_response()
    {
        return "HTTP/1.1 302 Found\r\n"
            "Location: http://7.7.7.7/config\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    std::string build_connectivity_check_response(const std::string& request)
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

    std::string connection_request_handler(const std::string& request, std::string& pass, std::string& ssid)
    {
        return "HTTP/1.1 202 Accepted\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    std::string build_freezer_config_page()
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

    std::string build_status_api_response(const double temperature, const double humidity)
    {
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
}
