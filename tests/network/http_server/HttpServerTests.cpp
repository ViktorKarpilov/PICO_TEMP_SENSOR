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

    TEST_F(HttpServerTests, ParseRequestSuccessfull) {
        char const *request_buffer = FORMDATA_REQUEST.data();
        HTTPMessage http_message;
    
        HttpServerHelpers::parse_request_package(request_buffer, http_message);
    
        EXPECT_EQ(http_message.body, R"(------WebKitFormBoundaryOzGKrWBdWbdXdz4GContent-Disposition: form-data; name="ssid"test_ssid------WebKitFormBoundaryOzGKrWBdWbdXdz4GContent-Disposition: form-data; name="password"test_password------WebKitFormBoundaryOzGKrWBdWbdXdz4G--)");
    }

    TEST_F(HttpServerTests, ParseSucessfullMessageOfFormDataRequest) {
        char const *request_buffer = FORMDATA_REQUEST.data();
        HTTPMessage http_message;

        HttpServerHelpers::parse_request_package(request_buffer, http_message);
        std::string pass;
        std::string ssid;

        auto response = HttpServerHelpers::connection_request_handler(http_message, pass, ssid);

        EXPECT_EQ(response, "HTTP/1.1 202 Accepted\r\n"
                  "Connection: close\r\n"
                  "\r\n");
        EXPECT_EQ(pass, "test_password");
        EXPECT_EQ(ssid, "test_ssid");
    }

    TEST_F(HttpServerTests, LineSpitWithCaretReturn) {
        auto result = HttpServerHelpers::split_by_lines(LINES_WIT_CARET_RETURN);
        
        EXPECT_EQ(result.size(), 4);
        EXPECT_EQ(result[0], "FirstLine");
    }

    TEST_F(HttpServerTests, LineSpitWithoutCaretReturn) {
        auto result = HttpServerHelpers::split_by_lines(LINES_WITHOUT_CARET_RETURN);
        
        EXPECT_EQ(result.size(), 4);
        EXPECT_EQ(result[0], "FirstLine");
    }
}
