#cmake_minimum_required(VERSION 3.31)
#
### Tell Pico SDK to find ARM toolchain automatically
##set(CMAKE_SYSTEM_NAME Generic)
##set(CMAKE_SYSTEM_PROCESSOR arm)
##set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
##
### Let SDK find ARM toolchain
##if(NOT DEFINED ENV{PICO_TOOLCHAIN_PATH})
##    set(ENV{PICO_TOOLCHAIN_PATH} "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.2 rel1/bin")
##endif()
###
## Project name
#project(TEMP_SENSOR_C)
#enable_language(C CXX ASM)
#
#set(PICO_USE_SW_SPIN_LOCK 0)
#add_compile_definitions(PICO_USE_SW_SPIN_LOCK=0)
#
## Include the Pico SDK import
#include(pico_sdk_import.cmake)
#
## Initialize the Pico SDK
#pico_sdk_init()
#
## Set C++ standard
#set(CMAKE_CXX_STANDARD 20)
#
## Create executable
#add_executable(TEMP_SENSOR_C main.cpp
#        src/main_loop.cpp
#        src/src.h
#        src/init.cpp
#        src/config.h
#        src/i2c_display/I2cDevice.cpp
#        src/i2c_display/I2cDevice.h
#        src/i2c_display/SSD1306.cpp
#        src/i2c_display/SSD1306.h)
#
## Link libraries
#target_link_libraries(TEMP_SENSOR_C pico_stdlib hardware_i2c)
#
### Enable UART output, disable USB output due to IDEs limitation (VS code and CLion just don't want to show anything from usb for some reason)
##pico_enable_stdio_usb(TEMP_SENSOR_C 0)
##pico_enable_stdio_uart(TEMP_SENSOR_C 1)
#
## Disable UF2 creation in pico_add_extra_outputs
##set(PICO_NO_UF2 0)
#
## Create standard outputs (.hex, .bin, .dis) - this builds picotool!
#pico_add_extra_outputs(TEMP_SENSOR_C)
#
## Add our custom .elf and .uf2 creation AFTER picotool is built
##add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
##        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE:${PROJECT_NAME}>.elf
##        COMMAND $<IF:$<TARGET_EXISTS:picotool>,picotool,$<TARGET_FILE_DIR:${PROJECT_NAME}>/../_deps/picotool-build/picotool> uf2 convert $<TARGET_FILE:${PROJECT_NAME}>.elf $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.uf2 --family rp2350
##        COMMENT "Creating .elf and .uf2 files for Wokwi"
##        VERBATIM
##)

cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(TEMP_SENSOR_C)
enable_language(C CXX ASM)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(TEMP_SENSOR_C main.cpp
        src/main_loop.cpp
        src/src.h
        src/init.cpp
        src/config.h
        src/i2c_display/I2cDevice.cpp
        src/i2c_display/I2cDevice.h
        src/i2c_display/SSD1306.cpp
        src/i2c_display/SSD1306.h
        lwipopts.h)

pico_enable_stdio_usb(TEMP_SENSOR_C 1)
pico_enable_stdio_uart(TEMP_SENSOR_C 0)

# Include current directory for lwipopts.h
target_include_directories(TEMP_SENSOR_C PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Link libraries
target_link_libraries(TEMP_SENSOR_C pico_stdlib hardware_i2c pico_cyw43_arch_lwip_poll)
pico_add_extra_outputs(TEMP_SENSOR_C)