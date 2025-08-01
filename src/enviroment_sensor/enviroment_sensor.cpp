//
// Created by PC on 7/9/2025.
//
#include "enviroment_sensor.h"

#include "src/config.h"
#include "BME280/BME280.h"
#include "hardware/iic/i2c.h"
#include "hardware/stdio/stdio.h"

void EnvironmentSensor::BME280_Read_NByte(const uint8_t RegAddr,uint8_t *buf, const uint8_t len) const
{
    sensor_i2c_write_blocking(CONFIG::I2CChip, this->address, &RegAddr, 1, true);
    sensor_i2c_read_blocking(CONFIG::I2CChip, this->address, buf, len, false);
}

int EnvironmentSensor::initiate()
{
    log("Environment sensor initialization started \r\n");

    //I2C Config
    gpio_set_function(CONFIG::SDA_PIN,GPIO_FUNC_I2C);
    gpio_set_function(CONFIG::SCL_PIN,GPIO_FUNC_I2C);
    gpio_pull_up(CONFIG::SDA_PIN);
    gpio_pull_up(CONFIG::SCL_PIN);

    log("BME280 initialization started \r\n");
    BME280_Init();

    log("DEV_Module_Init OK \r\n");
    return 0;
}

double EnvironmentSensor::readPressure()
{
    BME280_value();
    return pres_raw[0];
}

double EnvironmentSensor::readTemperature()
{
    BME280_value();
    return pres_raw[1];
}

double EnvironmentSensor::readHumidity()
{
    BME280_value();
    return pres_raw[2];
}
