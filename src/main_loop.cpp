#include <lwip/priv/tcp_priv.h>
#include <pico/cyw43_arch.h>

#include "src.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "config.h"
#include "enviroment_sensor/enviroment_sensor.h"
#include "spi_display/OLED13.h"
#include "spi_display/GUI/GUI_Paint.h"
#include "spi_display/OLED/OLED_1in3_c.h"

void loop()
{
    static uint32_t last_blink_time = 0;
    static uint32_t last_display_update = 0;
    static bool led_state = false;

    const uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Blink LED (non-blocking)
    if (current_time - last_blink_time >= CONFIG::DIODE_SLEEP_DURATION) {
        led_state = !led_state;
        gpio_put(CONFIG::RED_DIODE, led_state);
        last_blink_time = current_time;
    }
    
    // Update display every 500ms (so it's responsive but not too frequent)
    if (current_time - last_display_update >= 500) {
        Paint_DrawString_EN(5, 0, "Temperature: ", &Font12, WHITE, BLACK);
        Paint_DrawNum(90, 0, EnvironmentSensor::readTemperature(), &Font12, 2, WHITE, BLACK);
        
        Paint_DrawString_EN(5, 30, "Humidity: ", &Font12, WHITE, BLACK);
        Paint_DrawNum(90, 30, EnvironmentSensor::readHumidity(), &Font12, 2, WHITE, BLACK);
        
        OLED_1in3_C_Display(BlackImage);
        Paint_Clear(BLACK);
        
        last_display_update = current_time;
    }

    static uint32_t last_check = 0;
    if (to_ms_since_boot(get_absolute_time()) - last_check > 5000) {
        cyw43_arch_lwip_begin();
        if (netif_default) {
            printf("📊 Interface: UP=%s, LINK=%s, TCP_PCBs=%d\n",
                   netif_is_up(netif_default) ? "YES" : "NO",
                   netif_is_link_up(netif_default) ? "YES" : "NO",
                   tcp_active_pcbs ? 1 : 0);  // Check if TCP is working
        }
        cyw43_arch_lwip_end();
        last_check = to_ms_since_boot(get_absolute_time());
    }

    // In your main loop, check packet processing
    static uint32_t last_stats = 0;
    static uint32_t last_tcp_count = 0;

    if (to_ms_since_boot(get_absolute_time()) - last_stats > 5000) {
        uint32_t tcp_count = 0;
         tcp_pcb *pcb = tcp_active_pcbs;
        while (pcb) {
            tcp_count++;
            pcb = pcb->next;
        }
    
        if (tcp_count != last_tcp_count) {
            printf("📊 TCP PCBs changed: %d\n", tcp_count);
            last_tcp_count = tcp_count;
        }
        last_stats = to_ms_since_boot(get_absolute_time());
    }
    
    cyw43_arch_poll();  // Process Wi-Fi events
    sleep_ms(10);  // 10ms instead of 1000ms!
}
