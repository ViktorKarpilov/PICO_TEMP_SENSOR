//
// Created by PC on 7/9/2025.
//
#include "enviroment_sensor.h"

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "src/config.h"
#include "BME280/BME280.h"

void EnvironmentSensor::BME280_Read_NByte(const uint8_t RegAddr,uint8_t *buf, const uint8_t len) const
{
    i2c_write_blocking(CONFIG::I2CChip, this->address, &RegAddr, 1, true);
    i2c_read_blocking(CONFIG::I2CChip, this->address, buf, len, false);
}

int EnvironmentSensor::initiate()
{
    printf("Environment sensor initialization started \r\n");

    //I2C Config
    i2c_init(CONFIG::I2CChip,400*1000);
    gpio_set_function(CONFIG::SDA_PIN,GPIO_FUNC_I2C);
    gpio_set_function(CONFIG::SCL_PIN,GPIO_FUNC_I2C);
    gpio_pull_up(CONFIG::SDA_PIN);
    gpio_pull_up(CONFIG::SCL_PIN);

    printf("BME280 initialization started \r\n");
    BME280_Init();

    printf("DEV_Module_Init OK \r\n");
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

double EnvironmentSensor::readHumility()
{
    BME280_value();
    return pres_raw[2];
}
