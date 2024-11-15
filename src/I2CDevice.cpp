#include "I2CDevice.h"
#include <utility>

I2CDevice::I2CDevice(std::string  name, const byte address)
    : name(std::move(name)), address(address) {}

I2CDevice::~I2CDevice() = default;

std::string I2CDevice::getName() {
    return name;
}

byte I2CDevice::getAddress() {
    return address;
}
