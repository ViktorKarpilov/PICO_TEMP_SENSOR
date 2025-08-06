//
// Created by PC on 8/4/2025.
//

#include "HttpServerTests.h"

#include "../../../src/network/http_server/Helpers/HttpServerHelpers.h"
#include "gtest/gtest.h"
#include "../../data/FormDataExample.h"

namespace httpServerTests
{
    TEST_F(HttpServerTests, MethodBarDoesAbc) {
        auto result = HttpServerHelpers::is_android_internet_check("Some bla bla bla readaloud.googleapis.com blabla HTTP");
        EXPECT_EQ(result, true);
    }

    TEST_F(HttpServerTests, ParseSucessfullFormDataRequest) {
        std::string pass;
        std::string ssid;

        auto _ = HttpServerHelpers::connection_request_handler(FORMDATA_REQUEST, pass, ssid);
        
        EXPECT_EQ(pass, "test_password");
        EXPECT_EQ(ssid, "test_ssid");
    }

    TEST_F(HttpServerTests, ConnectionResponseValid) {
        std::string pass;
        std::string ssid;

        auto response = HttpServerHelpers::connection_request_handler(FORMDATA_REQUEST, pass, ssid);

        EXPECT_EQ(response, "HTTP/1.1 202 Accepted\r\n"
                  "Connection: close\r\n"
                  "\r\n");
    }
}
