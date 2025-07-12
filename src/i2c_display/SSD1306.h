#ifndef SSD1306_H
#define SSD1306_H
#include "I2cDevice.h"

class SSD1306 : I2cDevice {
private:
    bool is_turned_on = false;
public:
    explicit SSD1306(const uint8_t address) : I2cDevice(address) { }

    void initiate() const;
    int draw_vertical_line() const;
    void turn_on();
    void debug_addressing() const;
    void test_pixel_addressing() const;
    void test_simple_line() const;
};

#endif //SSD1306_H
