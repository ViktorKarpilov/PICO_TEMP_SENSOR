#ifndef CONFIG_H
#define CONFIG_H
#include "hardware/i2c.h"

namespace CONFIG
{
    constexpr int V3V_PIN  = 36;
    constexpr int UART_TX = 0;
    constexpr int UART_RX = 1;

    // Diode configs
    constexpr int RED_DIODE = 2;
    constexpr int GREEN_DIODE = 5;
    constexpr int SYSTEM_DIODE = 0;
    constexpr int DIODE_SLEEP_DURATION = 1000;

    // pico i2c config
    constexpr i2c_inst* I2CChip = i2c0;
    constexpr int SDA_PIN = 20;
    constexpr int SCL_PIN = 21;

    // pico for SSD1306 config
    constexpr int VCC_PIN  = V3V_PIN;
    constexpr int SSD1306_GND = 23;

    // SSD1306 config
    constexpr uint8_t SSD1306_ADDR = 0x3C;
}

#endif //CONFIG_H
