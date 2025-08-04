//
// Created by PC on 7/24/2025.
//

#include <src/config.h>
#include <vector>
#include "HttpServerHelpers.h"

namespace HttpServerHelpers
{
    inline std::vector<std::string> split_by_lines(const std::string& text)
    {
        std::vector<std::string> lines;
        size_t start = 0;
        size_t pos = 0;

        while ((pos = text.find("\r\n", start)) != std::string::npos)
        {
            lines.push_back(text.substr(start, pos - start));
            start = pos + 2;
        }

        if (start < text.length())
        {
            lines.push_back(text.substr(start));
        }

        return lines;
    }
}
