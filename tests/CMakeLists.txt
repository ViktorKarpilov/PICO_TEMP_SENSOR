# Auto-copy lwipcfg.h before building lwIP
set(LWIPCFG_EXAMPLE "${CMAKE_CURRENT_SOURCE_DIR}/../external/lwip/contrib/examples/example_app/lwipcfg.h.example")
set(LWIPCFG_TARGET "${CMAKE_CURRENT_SOURCE_DIR}/../external/lwip/contrib/examples/example_app/lwipcfg.h")

if(NOT EXISTS ${LWIPCFG_TARGET})
    configure_file(${LWIPCFG_EXAMPLE} ${LWIPCFG_TARGET} COPYONLY)
    message(STATUS "Auto-copied lwipcfg.h for lwIP")
endif()

include(CTest)

file(GLOB_RECURSE NETWORK_SRC "network/*.cpp")
add_executable(tests ${NETWORK_SRC})

target_include_directories(tests PRIVATE hardware_mocks)

target_link_libraries(tests
        PRIVATE GTest::gtest_main
        PRIVATE gtest
        PRIVATE lwipcore)

file(GLOB_RECURSE MOCK_SRC "hardware_mocks/*.cpp")
target_sources(tests PRIVATE ${MOCK_SRC})

include(GoogleTest)
gtest_discover_tests(tests)