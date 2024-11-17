#include "InputDevice.h"

InputDevice::InputDevice(std::string deviceName): name(deviceName){}


std::string InputDevice::getName() const
{
    return name;
}
