//
// Created by PC on 8/4/2025.
//

#ifndef HTTPSERVERHELPERS_H
#define HTTPSERVERHELPERS_H

#include <string>
#include <cstdint>
#include <vector>

enum HTTPRequestType
{
    GETTextHtml,
    POSTMultipart,
};

struct HTTPMessage
{
    std::string start_line;
    std::vector<std::string> headers;
    std::string body;
};

namespace HttpServerHelpers
{
    bool is_android_internet_check(const std::string& request);

    std::string build_captive_portal_response();

    std::string build_connectivity_check_response(const std::string& request);

    std::string connection_request_handler(const std::string& request, std::string& pass, std::string& ssid);

    std::string connection_request_handler(const HTTPMessage &message, std::string& pass, std::string& ssid);

    std::string build_freezer_config_page();

    std::string build_status_api_response(double temperature, double humidity);

    std::string build_get_ssids_response(const std::vector<std::string>& ssids);

    std::string build_error_response();

    std::vector<std::string> split_by_lines(const std::string& text);

    int parse_request_package(char const *request_buffer, HTTPMessage& message);
}

#endif //HTTPSERVERHELPERS_H
