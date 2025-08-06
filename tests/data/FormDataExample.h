//
// Created by PC on 8/5/2025.
//

#ifndef FORMDATAEXAMPLE_H
#define FORMDATAEXAMPLE_H
#include <string>

const std::string FORMDATA_REQUEST = R"(POST / HTTP/1.1
Host: localhost:8080
Connection: keep-alive
Content-Length: 238
sec-ch-ua-platform: "Windows"
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36
sec-ch-ua: "Not)A;Brand";v="8", "Chromium";v="138", "Brave";v="138"
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryOzGKrWBdWbdXdz4G
sec-ch-ua-mobile: ?0
Accept: */*
Sec-GPC: 1
Origin: null
Sec-Fetch-Site: cross-site
Sec-Fetch-Mode: cors
Sec-Fetch-Dest: empty
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: en-US,en;q=0.9

------WebKitFormBoundaryOzGKrWBdWbdXdz4G
Content-Disposition: form-data; name="ssid"

test_ssid
------WebKitFormBoundaryOzGKrWBdWbdXdz4G
Content-Disposition: form-data; name="password"

test_password
------WebKitFormBoundaryOzGKrWBdWbdXdz4G--
)";

#endif //FORMDATAEXAMPLE_H
