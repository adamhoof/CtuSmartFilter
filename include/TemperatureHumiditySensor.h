#pragma once
#include "I2CDevice.h"

class TemperatureHumiditySensor : public I2CDevice {
public:
    using I2CDevice::I2CDevice;
};
