//
// Created by PC on 7/24/2025.
//

#include <src/config.h>
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

inline std::vector<std::string> split_by_lines(const std::string& text) {
    std::vector<std::string> lines;
    size_t start = 0;
    size_t pos = 0;
    
    while ((pos = text.find("\r\n", start)) != std::string::npos) {
        lines.push_back(text.substr(start, pos - start));
        start = pos + 2;
    }
    
    if (start < text.length()) {
        lines.push_back(text.substr(start));
    }
    
    return lines;
}

static int parse_request_package(const pbuf* package, HTTPMessage& message)
{
    char request_buffer[CONFIG::MAX_REQUEST_SIZE];
    
    uint16_t copied = pbuf_copy_partial(package, request_buffer, 
                                       package->tot_len, 0);
    
    request_buffer[copied] = '\0';
    const std::string content(request_buffer);

    const auto lines = split_by_lines(content);

    int start_index = 0;
    bool found_body = !message.body.empty();

    if (!found_body && message.start_line.empty())
    {
        message.start_line = lines[0];
        start_index = 1;
    }

    for (int i = start_index; i < lines.size(); i++)
    {
        if (!found_body)
        {
            if (lines[i].empty())
            {
                found_body = true;
                continue;
            }
            message.headers.push_back(lines[i]);
            continue;
        }

        message.body += lines[i];
    }

    return ERR_OK;
}