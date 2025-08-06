//
// Created by PC on 8/4/2025.
//

#include <format>
#include <string>
#include <generated/config_html.h>
#include "HttpServerHelpers.h"
#include <regex>
using namespace std;

namespace HttpServerHelpers
{
    bool is_android_internet_check(const string& request)
    {
        return request.find("connectivitycheck.gstatic.com") != string::npos ||
            request.find("generate_204") != string::npos ||
            request.find("readaloud.googleapis.com") != string::npos ||
            request.find("clients4.google.com") != string::npos ||
            request.find("alt3-mtalk.google.com") != string::npos;
    }

    string build_error_response()
    {
        return "HTTP/1.1 400 Bad Request\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    string build_captive_portal_response()
    {
        return "HTTP/1.1 302 Found\r\n"
            "Location: http://7.7.7.7/config\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    string build_connectivity_check_response(const string& request)
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

    void clean_body_value(string& body) {
        // Remove first 3 lines
        for (int i = 0; i < 3; ++i) {
            size_t pos = body.find('\n');
            if (pos != string::npos) body = body.substr(pos + 1);
        }
        // Remove last line
        size_t last_nl = body.find_last_of('\n');
        if (last_nl != string::npos) body = body.substr(0, last_nl);
        // Trim
        body.erase(0, body.find_first_not_of(" \t\r\n"));
        body.erase(body.find_last_not_of(" \t\r\n") + 1);
    }

    string connection_request_handler(const string& request, string& pass, string& ssid)
    {
        const regex delimiter_regexp(R"(boundary=([^\s;]+))");  // Fixed spelling + better pattern
        smatch match;

        if (!regex_search(request, match, delimiter_regexp) || match.size() != 2) {
            return build_error_response();
        }

        const string delimiter = match[1].str();
        const string pattern = "--" + delimiter + R"(([\s\S]*?)(?=------))";  // Fixed: 6 dashes
        const regex groups_regexp(pattern);

        sregex_iterator iter(request.begin(), request.end(), groups_regexp);
        sregex_iterator end;

        constexpr int expected_count = 2;
        if (distance(iter, end) != expected_count) {
            return build_error_response(); 
        }

        string form_fields[expected_count];
        for (int i = 0; i < expected_count && iter != end; ++i, ++iter) {
            form_fields[i] = (*iter)[1].str();
            clean_body_value(form_fields[i]);
        }

        ssid = form_fields[0];
        pass = form_fields[1];

        return "HTTP/1.1 202 Accepted\r\n"
               "Connection: close\r\n"
               "\r\n";
    }

    string build_freezer_config_page()
    {
        const string& html = HtmlResources::CONFIG_PAGE;

        // CRITICAL: Ensure exact byte count
        size_t actual_length = html.length();

        return "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(actual_length) + "\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: close\r\n"
            "\r\n" + html;
    }

    string build_status_api_response(const double temperature, const double humidity)
    {
        string json = "{"
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
