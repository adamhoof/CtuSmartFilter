#include "InputDevice.h"

InputDevice::InputDevice(const std::string& deviceName): name(deviceName){}


std::string InputDevice::getName() const
{
    return name;
}
