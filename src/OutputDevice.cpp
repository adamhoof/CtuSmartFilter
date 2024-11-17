#include "OutputDevice.h"
#include <utility>

OutputDevice::OutputDevice(std::string deviceName) : name(std::move(deviceName))
{
}

std::string OutputDevice::getName() const
{
    return name;
}
