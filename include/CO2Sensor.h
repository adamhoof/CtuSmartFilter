#pragma once
#include "I2CDevice.h"

class CO2Sensor : public I2CDevice {
public:
    using I2CDevice::I2CDevice;
};
