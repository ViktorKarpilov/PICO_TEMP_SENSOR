//
// Created by PC on 7/26/2025.
//
#include <gtest/gtest.h>

namespace HTTPServerTests
{
    int testRet()
    {
        return 42;
    }
}

TEST(Topic, NameOfTest)
{
    EXPECT_EQ(HTTPServerTests::testRet(), 42);
}