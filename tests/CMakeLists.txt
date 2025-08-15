set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 11.0)

cmake_minimum_required(VERSION 3.13)

project(TEMP_SENSOR_TESTS)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

add_executable(TEMP_SENSOR_TESTS main.cpp
        network/http_server/HttpServerTests.cpp
        network/http_server/HttpServerTests.h

        ../src/network/http_server/Helpers/HttpServerHelpers.h
        ../src/network/http_server/Helpers/HTTPHelper.cpp
        ../src/network/http_server/Helpers/HTTPResponseBuilder.cpp)

target_include_directories(TEMP_SENSOR_TESTS PRIVATE
        ../src                  
        ../generated           
        ..                    
)

add_subdirectory(external/googletest)

target_link_libraries(TEMP_SENSOR_TESTS PRIVATE GTest::gtest_main gtest)

include(GoogleTest)
gtest_discover_tests(TEMP_SENSOR_TESTS)