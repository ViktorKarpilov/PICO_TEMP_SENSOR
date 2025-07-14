cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)
set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2_w)
set(PICO_NO_PICOTOOL TRUE)

set(picotool_EXECUTABLE "picotool")

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(TEMP_SENSOR_C)
enable_language(C CXX ASM)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

message(STATUS "=== COMPILER INSPECTION ===")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
message(STATUS "CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================")
# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

execute_process(
        COMMAND python ${CMAKE_SOURCE_DIR}/tools/html_to_cpp.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(TEMP_SENSOR_C main.cpp lwipopts.h generated/config_html.h generated/status_html.h)

pico_enable_stdio_usb(TEMP_SENSOR_C 1)
pico_enable_stdio_uart(TEMP_SENSOR_C 0)

file(GLOB_RECURSE SRC "src/*.cpp")

target_sources(TEMP_SENSOR_C PRIVATE ${SRC})

# Include current directory for lwipopts.h
target_include_directories(TEMP_SENSOR_C PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Link libraries
target_link_libraries(TEMP_SENSOR_C pico_stdlib pico_cyw43_arch_lwip_poll hardware_spi hardware_i2c hardware_pwm)
pico_add_extra_outputs(TEMP_SENSOR_C)

# It always tries to rebuild picotool and can't - easier just to do it by hands
add_custom_command(TARGET TEMP_SENSOR_C POST_BUILD
        COMMAND picotool uf2 convert ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.elf ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.uf2
        COMMENT "Converting ELF to UF2 using system picotool"
        VERBATIM
)
