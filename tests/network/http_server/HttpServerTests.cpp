//
// Created by PC on 8/4/2025.
//

#include "HttpServerTests.h"

#include "../../../src/network/http_server/Helpers/HttpServerHelpers.h"
#include "gtest/gtest.h"

namespace httpServerTests
{
    TEST_F(HttpServerTests, MethodBarDoesAbc) {
        auto result = HttpServerHelpers::is_android_internet_check("Some bla bla bla readaloud.googleapis.com blabla HTTP");
        EXPECT_EQ(result, true);
    }
}
