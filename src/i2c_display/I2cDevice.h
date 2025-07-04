#ifndef I2CDEVICE_H
#define I2CDEVICE_H
#include <cstdint>

class I2cDevice {
private:
    uint8_t address;

public:
    explicit I2cDevice(const uint8_t address) : address{address}
    { };

    void sendCommand(uint8_t command) const;
    void sendData(const uint8_t* data, long len) const;
};

#endif //I2CDEVICE_H
