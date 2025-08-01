#include "I2cDevice.h"

#include <cstdio>

#include "../config.h"
#include "hardware/iic/i2c.h"
#include "hardware/stdio/stdio.h"

#define COMMAND_CONTROL_BYTE 0x00
#define DATA_CONTROL_BYTE 0x40

void I2cDevice::sendCommand(const uint8_t command) const
{
    const uint8_t data[] = {COMMAND_CONTROL_BYTE, command};
    const int result = sensor_i2c_write_blocking(CONFIG::I2CChip, address, data, 2, false);
    log("I2C write result: %d (should be 2)\n", result);
}

void I2cDevice::sendData(const uint8_t* data, const long len) const
{
    const auto buffer = new uint8_t[len + 1];
    buffer[0] = DATA_CONTROL_BYTE;

    for(size_t i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }

    sensor_i2c_write_blocking(CONFIG::I2CChip, address, buffer, len + 1, false);
    delete[] buffer;
}
