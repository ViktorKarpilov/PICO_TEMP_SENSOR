//
// Created by PC on 7/24/2025.
//

#include <vector>
#include "HttpServerHelpers.h"

namespace HttpServerHelpers
{
    std::vector<std::string> split_by_lines(const std::string& text)
    {
        std::vector<std::string> lines;
        size_t start = 0;
        size_t pos = 0;

        // Default delimiter
        int new_line_delimiter_size = 1;
        std::string new_line_delimiter = "\n";

        if (text.find("\r\n") != std::string::npos)
        {
            new_line_delimiter = "\r\n";
            new_line_delimiter_size = 2;
        }

        while ((pos = text.find(new_line_delimiter, start)) != std::string::npos)
        {
            lines.push_back(text.substr(start, pos - start));
            start = pos + new_line_delimiter_size;
        }

        if (start < text.length())
        {
            lines.push_back(text.substr(start));
        }

        return lines;
    }
    
    int parse_request_package(char const *request_buffer, HTTPMessage& message)
    {
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

        return 0;
    }
}
