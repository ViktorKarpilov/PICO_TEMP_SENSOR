#include "src.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "config.h"

void loop()
{
    gpio_put(LED::RED_DIOD, true);
    sleep_ms(LED::DIOD_SLEEP_DURATION);
    gpio_put(LED::RED_DIOD, false);
    sleep_ms(LED::DIOD_SLEEP_DURATION);
}
