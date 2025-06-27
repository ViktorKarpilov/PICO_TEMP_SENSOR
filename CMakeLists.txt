cmake_minimum_required(VERSION 3.31)
# Project name
project(TEMP_SENSOR_C)
enable_language(C CXX ASM)

# Include the Pico SDK import
include(pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Create executable
add_executable(TEMP_SENSOR_C main.cpp
        src/main_loop.cpp
        src/src.h
        src/init.cpp
        src/config.h)

# Link libraries
target_link_libraries(TEMP_SENSOR_C pico_stdlib)

# Enable USB output, disable UART output
pico_enable_stdio_usb(TEMP_SENSOR_C 1)
pico_enable_stdio_uart(TEMP_SENSOR_C 0)

# Disable UF2 creation in pico_add_extra_outputs
set(PICO_NO_UF2 1)

# Create standard outputs (.hex, .bin, .dis) - this builds picotool!
pico_add_extra_outputs(TEMP_SENSOR_C)

# Add our custom .elf and .uf2 creation AFTER picotool is built
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE:${PROJECT_NAME}>.elf
        COMMAND $<IF:$<TARGET_EXISTS:picotool>,picotool,$<TARGET_FILE_DIR:${PROJECT_NAME}>/../_deps/picotool-build/picotool> uf2 convert $<TARGET_FILE:${PROJECT_NAME}>.elf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.uf2 --family rp2040
        COMMENT "Creating .elf and .uf2 files for Wokwi"
        VERBATIM
)