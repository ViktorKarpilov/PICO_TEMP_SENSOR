#include "config.h"
#include "src.h"
#include "hardware/gpio.h"

void init()
{
    gpio_init(LED::RED_DIOD);
    gpio_set_dir(LED::RED_DIOD, GPIO_OUT);
}