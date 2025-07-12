#include "pico/stdlib.h"
#include <cstdio>

#include "src/config.h"
#include "src/src.h"

[[noreturn]] int main() {
     if (init())
     {
         printf("failed to initialize\n");
         while (true)
         {
             sleep_ms(100);
             gpio_put(CONFIG::RED_DIODE, !gpio_get(CONFIG::RED_DIODE));
         }
     }
    
    gpio_put(CONFIG::GREEN_DIODE, true);
    printf("=== INITIALIZATION COMPLETE ===\n");
    printf("If you see this, stdio works!\n");
    
    while (true)
    {
        printf("Loop\n");
        loop();
    }
}
