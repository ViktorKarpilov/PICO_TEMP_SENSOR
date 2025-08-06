//
// Created by PC on 8/4/2025.
//

#ifndef HTTPSERVERHELPERS_H
#define HTTPSERVERHELPERS_H

#include <string>
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

    std::string build_freezer_config_page();

    std::string build_status_api_response(const double temperature, const double humidity);

    std::vector<std::string> split_by_lines(const std::string& text);
}

#endif //HTTPSERVERHELPERS_H
