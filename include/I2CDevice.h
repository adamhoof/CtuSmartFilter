#pragma once
#include <Arduino.h>
#include <string>

class I2CDevice {
public:
    I2CDevice(std::string  name, byte address);
    virtual ~I2CDevice();

    virtual std::string getName();
    virtual byte getAddress();

protected:
    std::string name;
    byte address;
};
