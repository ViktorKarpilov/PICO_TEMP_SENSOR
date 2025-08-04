//
// Created by PC on 8/4/2025.
//

#include <gtest/gtest.h>

namespace my::project
{
    namespace {
        // The fixture for testing class Foo.
        class ExampleTest : public testing::Test {
        protected:
            // You can remove any or all of the following functions if their bodies would
            // be empty.

            ExampleTest() {
                // You can do set-up work for each test here.
            }

            ~ExampleTest() override {
                // You can do clean-up work that doesn't throw exceptions here.
            }

            // If the constructor and destructor are not enough for setting up
            // and cleaning up each test, you can define the following methods:

            void SetUp() override {
                // Code here will be called immediately after the constructor (right
                // before each test).
            }

            void TearDown() override {
                // Code here will be called immediately after each test (right
                // before the destructor).
            }

            // Class members declared here can be used by all tests in the test suite
            // for Foo.
        };

        TEST_F(ExampleTest, MethodBarDoesAbc) {
            EXPECT_EQ(0, 0);
        }
    }  // namespace
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
