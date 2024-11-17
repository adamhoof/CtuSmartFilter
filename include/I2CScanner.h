#pragma once
#include <vector>
#include <string>
#include <functional>
#include "I2CDevice.h"

class I2CScanner
{
    std::vector<std::reference_wrapper<I2CDevice>> i2cDevices;

public:
    I2CScanner();

    void addDevice(I2CDevice& i2cDevice);

    void addDevices(const std::vector<std::reference_wrapper<I2CDevice>>& devicesList);

    enum status { SUCCESS, FAILURE };
    struct ScanResult
    {
        status resultStatus;
        std::string message;
    };

    std::vector<ScanResult> scan() const;
};
