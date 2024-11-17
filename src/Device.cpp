#include "Device.h"

#include <HardwareSerial.h>

Device::Device(const std::string&  deviceName) : name(deviceName) {}

std::string Device::getName() const {
    return name;
}
