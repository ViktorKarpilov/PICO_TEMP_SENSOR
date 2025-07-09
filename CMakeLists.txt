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
set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2_w)
set(PICO_NO_PICOTOOL TRUE)

#set(CMAKE_HOST_SYSTEM_NAME Windows)
#set(PICO_NO_PICOTOOL TRUE)
#set(PICO_BUILD_DOCS FALSE)
#set(CMAKE_CROSSCOMPILING TRUE)

# Cross compiling compilators
#set(PICO_HOST_CXX_COMPILER "g++.exe")
#set(PICO_HOST_C_COMPILER "gcc.exe")

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

## Collect sources from each directory
#file(GLOB MAIN_SOURCES "*.cpp")
#file(GLOB SRC_SOURCES "src/*.cpp" "src/*.c")
#file(GLOB I2C_SOURCES "src/i2c_display/*.cpp")
#file(GLOB SPI_SOURCES "src/spi_display/*.cpp")
#
#add_executable(TEMP_SENSOR_C
#        ${MAIN_SOURCES}
#        ${SRC_SOURCES}
#        ${I2C_SOURCES}
#        ${SPI_SOURCES}
#)

add_executable(TEMP_SENSOR_C main.cpp
        src/main_loop.cpp
        src/src.h
        src/init.cpp
        src/config.h
        src/i2c_display/I2cDevice.cpp
        src/i2c_display/I2cDevice.h
        src/i2c_display/SSD1306.cpp
        src/i2c_display/SSD1306.h
        lwipopts.h
        src/spi_display/OLED13.cpp
        src/spi_display/OLED13.h
        src/spi_display/ImageData.cpp
        src/spi_display/ImageData.h
        src/enviroment_sensor/enviroment_sensor.h
        src/enviroment_sensor/enviroment_sensor.cpp)
#        src/spi_display/Config/DEV_Config.c
#        src/spi_display/Config/Debug.h)

pico_enable_stdio_usb(TEMP_SENSOR_C 1)
pico_enable_stdio_uart(TEMP_SENSOR_C 0)

# Add all the library files automatically
file(GLOB_RECURSE WAVESHARE_LIB_SOURCES "src/spi_display/*/*.cpp")
file(GLOB_RECURSE WAVESHARE_HEADERS "src/spi_display/**/*.h")

file(GLOB_RECURSE ENVIRONMENT_SENSOR_SOURCES "src/enviroment_sensor/*.cpp")


# Debug what files were found
message(STATUS "=== FOUND FILES ===")
foreach(file ${ENVIRONMENT_SENSOR_SOURCES})
    message(STATUS "Found: ${file}")
endforeach()
message(STATUS "Total files found: ${CMAKE_CURRENT_LIST_LENGTH}")
message(STATUS "==================")

target_sources(TEMP_SENSOR_C PRIVATE ${WAVESHARE_LIB_SOURCES} ${WAVESHARE_HEADERS} )
target_sources(TEMP_SENSOR_C PRIVATE ${ENVIRONMENT_SENSOR_SOURCES})

# Include current directory for lwipopts.h
target_include_directories(TEMP_SENSOR_C PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Link libraries
target_link_libraries(TEMP_SENSOR_C pico_stdlib pico_cyw43_arch_lwip_poll hardware_spi hardware_i2c hardware_pwm)
pico_add_extra_outputs(TEMP_SENSOR_C)

# It always tries to rebuild picotool and can't - easier just to do it by hands
add_custom_command(TARGET TEMP_SENSOR_C POST_BUILD
        COMMAND picotool uf2 convert ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.elf ${CMAKE_CURRENT_BINARY_DIR}/TEMP_SENSOR_C.uf2
        COMMENT "Converting ELF to UF2 using system picotool"
        VERBATIM
)
