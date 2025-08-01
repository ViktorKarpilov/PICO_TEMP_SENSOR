cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)
set(PICO_NO_PICOTOOL TRUE)
set(PICO_BUILD_DOCS FALSE)
set(picotool_EXECUTABLE "picotool")

if(NOT BUILD_TESTING)
    set(PICO_PLATFORM rp2350 CACHE INTERNAL "Platform" FORCE)
    set(PICO_BOARD pico2_w CACHE INTERNAL "Board" FORCE)
endif ()

# pico_sdk_import.cmake is a single file copied from this SDK
if (NOT BUILD_TESTING)
    include(pico_sdk_import.cmake)
endif ()

project(TEMP_SENSOR_C)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 20)

if (BUILD_TESTING)
    set(PROJECT_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Project config directory")
# Just so happen currently used only for tests
    add_subdirectory(external)
    add_subdirectory(tests)
else()
    message(STATUS "INIT PICO SDK")
    pico_sdk_init()
endif ()

message(STATUS "EXECUTABLE ")
add_executable(TEMP_SENSOR_C main.cpp lwipopts.h generated/config_html.h generated/status_html.h
        hardware/stdio/stdio.h
        hardware/iic/i2c.h
        hardware/pico/pico.h)

if (NOT BUILD_TESTING)
    message(STATUS "NOT BUILD TESTING")
    target_link_libraries(TEMP_SENSOR_C
            pico_stdlib
            pico_cyw43_arch_lwip_poll
            hardware_spi
            hardware_i2c
            hardware_pwm)
    pico_enable_stdio_usb(TEMP_SENSOR_C 1)
    pico_enable_stdio_uart(TEMP_SENSOR_C 0)
    pico_add_extra_outputs(TEMP_SENSOR_C)
endif ()

message(STATUS "=== COMPILER INSPECTION ===")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
message(STATUS "CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================")


file(GLOB_RECURSE SRC "src/*.cpp")
target_sources(TEMP_SENSOR_C PRIVATE ${SRC})

# Include current directory for lwipopts.h
target_include_directories(TEMP_SENSOR_C PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Link libraries
if(NOT BUILD_TESTING) 
    # Only convert to UF2 for firmware builds
    add_custom_command(TARGET TEMP_SENSOR_C POST_BUILD
            COMMAND picotool uf2 convert ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.elf ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.uf2
            COMMENT "Converting ELF to UF2 using system picotool"
            VERBATIM)
endif()