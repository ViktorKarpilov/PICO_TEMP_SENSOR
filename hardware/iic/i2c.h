//
// Created by PC on 8/1/2025.
//

#ifndef I2C_H
#define I2C_H
#include "hardware/i2c.h"

// ===== TYPE ALIASES =====
using sensor_i2c_inst_t = i2c_inst_t;
using sensor_i2c_inst_ptr = i2c_inst_t*;

// ===== CONSTANTS ALIASES =====
constexpr auto SENSOR_I2C0 = i2c0;
constexpr auto SENSOR_I2C1 = i2c1;

// Return value constants (assuming PICO SDK uses these)
constexpr auto SENSOR_I2C_SUCCESS = 0;
constexpr auto SENSOR_I2C_ERROR = -1;
constexpr auto SENSOR_I2C_TIMEOUT = -2;

// ===== FUNCTION POINTER ALIASES =====
using sensor_i2c_init_fn = uint (*)(i2c_inst_t*, uint);
using sensor_i2c_write_blocking_fn = int (*)(i2c_inst_t*, uint8_t, const uint8_t*, size_t, bool);
using sensor_i2c_read_blocking_fn = int (*)(i2c_inst_t*, uint8_t, uint8_t*, size_t, bool);
using sensor_i2c_deinit_fn = void (*)(i2c_inst_t*);

// ===== FUNCTION ALIASES =====
#define sensor_i2c_init(i2c, baudrate) i2c_init(i2c, baudrate)
#define sensor_i2c_write_blocking(i2c, addr, src, len, nostop) i2c_write_blocking(i2c, addr, src, len, nostop)
#define sensor_i2c_read_blocking(i2c, addr, dst, len, nostop) i2c_read_blocking(i2c, addr, dst, len, nostop)
#define sensor_i2c_deinit(i2c) i2c_deinit(i2c)

// ===== CONVENIENCE FUNCTIONS =====
inline int sensor_i2c_write_reg(sensor_i2c_inst_ptr i2c, uint8_t device_addr, uint8_t reg_addr, const uint8_t* data, size_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg_addr;
    for (size_t i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }
    return sensor_i2c_write_blocking(i2c, device_addr, buffer, len + 1, false);
}

inline int sensor_i2c_read_reg(sensor_i2c_inst_ptr i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t* data, size_t len) {
    // Write register address
    int result = sensor_i2c_write_blocking(i2c, device_addr, &reg_addr, 1, true);
    if (result < 0) return result;
    
    // Read data
    return sensor_i2c_read_blocking(i2c, device_addr, data, len, false);
}

inline int sensor_i2c_write_byte(sensor_i2c_inst_ptr i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t value) {
    uint8_t data[2] = {reg_addr, value};
    return sensor_i2c_write_blocking(i2c, device_addr, data, 2, false);
}

inline uint8_t sensor_i2c_read_byte(sensor_i2c_inst_ptr i2c, uint8_t device_addr, uint8_t reg_addr) {
    uint8_t value = 0;
    sensor_i2c_read_reg(i2c, device_addr, reg_addr, &value, 1);
    return value;
}


#endif //I2C_H
