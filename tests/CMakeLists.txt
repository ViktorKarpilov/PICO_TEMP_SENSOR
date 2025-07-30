if (BUILD_TESTING)
    add_executable(tests network/http_server/http_server_tests.cpp
            mocks/lwip/tcp.cpp
            mocks/lwip/tcp.h)
    target_link_libraries(tests PRIVATE GTest::gtest_main gtest)
    
    include(GoogleTest)
    gtest_discover_tests(tests)
endif ()