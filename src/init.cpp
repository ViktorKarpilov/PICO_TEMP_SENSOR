#include <cstdio>
#include <hardware/uart.h>

#include "config.h"
#include "src.h"
#include "enviroment_sensor/enviroment_sensor.h"
#include "hardware/pico/pico.h"
#include "hardware/stdio/stdio.h"
#include "spi_display/OLED13.h"
#include "network/wifi_service/WifiService.h"

static WifiService wifi_service;

int init()
{
    sensor_stdio_init_all();
    gpio_init(CONFIG::RED_DIODE);
    gpio_set_dir(CONFIG::RED_DIODE, GPIO_OUT);
    gpio_put(CONFIG::RED_DIODE, true);

    // Initialize the CYW43 architecture
    if (cyw43_arch_init()) {
        log("Failed to initialize CYW43\n");
        return -1;
    }

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    constexpr uint32_t timeout_ms = 10000; // 10 second timeout
    
    while (!sensor_stdio_usb_connected()) {
        sleep_ms(100);
        
        // Don't wait forever
        if (to_ms_since_boot(get_absolute_time()) - start_time > timeout_ms) {
            log("USB timeout - proceeding anyway\n");
            break;
        }
    }
    
    if (sensor_stdio_usb_connected()) {
        log("=== USB CONNECTED AND READY! ===\n");
    } else {
        log("=== NO USB - USING FALLBACK ===\n");
    }

    if (OLED_Init())
    {
        log("OLED initialization error\n");
        return 1;
    }
    
    if (EnvironmentSensor::initiate())
    {
        log("envSensor initialization error\n");
        return 1;
    }

    log("AP initiation \n");

    wifi_service.turn_on_captive_portal();

    return 0;
}

void setupUART()
{
    uart_init(uart0, 115200);
    gpio_set_function(CONFIG::UART_TX, GPIO_FUNC_UART);
    gpio_set_function(CONFIG::UART_RX, GPIO_FUNC_UART);
    
    stdio_init_all();
    log("UART initiated \n");
}
