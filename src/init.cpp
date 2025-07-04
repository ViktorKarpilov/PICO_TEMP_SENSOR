#include <cstdio>
#include <hardware/uart.h>
#include <pico/stdio.h>

#include "config.h"
#include "src.h"
#include "hardware/gpio.h"
#include "i2c_display/SSD1306.h"
#include "pico/cyw43_arch.h"
#include "pico/stdio_usb.h"

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

    return 0;

    // Initialize I2C at 400kHz
    // i2c_init(CONFIG::I2CChip, 400 * 1000);
    // gpio_set_function(CONFIG::SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(CONFIG::SCL_PIN, GPIO_FUNC_I2C);
    //
    // SSD1306 display(CONFIG::SSD1306_ADDR);
    // display.turn_on();
}

void setupUART()
{
    uart_init(uart0, 115200);
    gpio_set_function(CONFIG::UART_TX, GPIO_FUNC_UART);
    gpio_set_function(CONFIG::UART_RX, GPIO_FUNC_UART);
    
    stdio_init_all();
    printf("UART initiated \n");
}
