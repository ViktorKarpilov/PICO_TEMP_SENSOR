#include <cstdio>
#include <hardware/uart.h>
#include <pico/stdio.h>

#include "config.h"
#include "src.h"
#include "enviroment_sensor/enviroment_sensor.h"
#include "hardware/gpio.h"
#include "i2c_display/SSD1306.h"
#include "pico/cyw43_arch.h"
#include "pico/stdio_usb.h"
#include "spi_display/OLED13.h"
#include "spi_display/GUI/GUI_Paint.h"
#include "spi_display/OLED/OLED_1in3_c.h"

int init()
{
    stdio_init_all();
    gpio_init(CONFIG::RED_DIODE);
    gpio_set_dir(CONFIG::RED_DIODE, GPIO_OUT);
    gpio_put(CONFIG::RED_DIODE, true);

    // Initialize the CYW43 architecture (essential for Pico W/2W)
    if (cyw43_arch_init()) {
        printf("Failed to initialize CYW43\n");
        return -1;
    }

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    constexpr uint32_t timeout_ms = 10000; // 10 second timeout
    
    while (!stdio_usb_connected()) {
        sleep_ms(100);
        
        // Don't wait forever
        if (to_ms_since_boot(get_absolute_time()) - start_time > timeout_ms) {
            printf("USB timeout - proceeding anyway\n");
            break;
        }
    }
    
    if (stdio_usb_connected()) {
        printf("=== USB CONNECTED AND READY! ===\n");
    } else {
        printf("=== NO USB - USING FALLBACK ===\n");
    }

    OLED_Init();
    // OLED_1in3_C_test();
    
    // const auto envSensor = EnvironmentSensor(0x76);
    if (EnvironmentSensor::initiate())
    {
        printf("envSensor initialization error\n");
        return 1;
    }

    printf("envSensor initialization done\n");
    // EnvironmentSensor::initiate();
    const double temp = EnvironmentSensor::readTemperature();

    printf("Temperature: %f\n", temp);
    Paint_DrawString_EN(10, 0, "Temperature: ", &Font16, WHITE, BLACK);
    Paint_DrawNum(10, 30, temp, &Font8,2, WHITE, BLACK);
    
    return 0;
}

void setupUART()
{
    uart_init(uart0, 115200);
    gpio_set_function(CONFIG::UART_TX, GPIO_FUNC_UART);
    gpio_set_function(CONFIG::UART_RX, GPIO_FUNC_UART);
    
    stdio_init_all();
    printf("UART initiated \n");
}
