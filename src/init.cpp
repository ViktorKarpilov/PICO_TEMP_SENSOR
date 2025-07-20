#include <cstdio>
#include <hardware/uart.h>
#include <pico/stdio.h>

#include "config.h"
#include "src.h"
#include "enviroment_sensor/enviroment_sensor.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "pico/stdio_usb.h"
#include "spi_display/OLED13.h"
#include "network/wifi_service/WifiService.h"

static WifiService wifi_service;

int init()
{
    stdio_init_all();
    gpio_init(CONFIG::RED_DIODE);
    gpio_set_dir(CONFIG::RED_DIODE, GPIO_OUT);
    gpio_put(CONFIG::RED_DIODE, true);

    // Initialize the CYW43 architecture
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

    if (OLED_Init())
    {
        printf("OLED initialization error\n");
        return 1;
    }
    
    if (EnvironmentSensor::initiate())
    {
        printf("envSensor initialization error\n");
        return 1;
    }

    printf("AP initiation \n");

    wifi_service.turn_on_captive_portal();

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
