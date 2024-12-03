#include "OutputDevice.h"

OutputDevice::OutputDevice(std::string deviceName)
    : name(std::move(deviceName)), dataMutex(xSemaphoreCreateMutex()) {}

OutputDevice::~OutputDevice() {
    if (dataMutex) {
        vSemaphoreDelete(dataMutex);
    }
}

std::string OutputDevice::getName() const
{
    return name;
}
