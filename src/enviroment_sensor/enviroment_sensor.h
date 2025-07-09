//
// Created by PC on 7/9/2025.
//

#ifndef ENVIROMENT_SENSOR_H
#define ENVIROMENT_SENSOR_H
#include <cstdint>
#include <cstdio>
#include <stdexcept>

class EnvironmentSensor
{
    uint8_t address;
    void BME280_Read_NByte(uint8_t RegAddr, uint8_t* buf, uint8_t len) const;

    public:
        explicit EnvironmentSensor(uint8_t address)
        {
            this->address = address;
        };

        [[nodiscard]] static int initiate();
        [[nodiscard]] static double readTemperature();
        [[nodiscard]] static double readPressure();
        [[nodiscard]] static double readHumility();
};

#endif //ENVIROMENT_SENSOR_H
