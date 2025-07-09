#include "src.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "config.h"

void loop()
{
    gpio_put(CONFIG::RED_DIODE, !gpio_get(CONFIG::RED_DIODE));
    sleep_ms(CONFIG::DIODE_SLEEP_DURATION);
}
