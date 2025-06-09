#include "Device.h"

Device::Device(const char* deviceName) : deviceName(deviceName) {
}

const char* Device::getName() const {
    return this->deviceName;
}