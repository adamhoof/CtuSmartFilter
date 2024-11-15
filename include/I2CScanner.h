#pragma once

#include <I2CDevice.h>
#include <memory>
#include <vector>

class I2CScanner {
public:
    I2CScanner();
    void addDevice(I2CDevice& i2cDevice);
    void addDevices(const std::vector<std::reference_wrapper<I2CDevice>>& devicesList);
    void scan();

private:
    std::vector<std::reference_wrapper<I2CDevice>> i2cDevices;
};
