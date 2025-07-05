#include "SensorDevice.h"
#include "InvalidValue.h"
#include "LockGuard.h"

inline std::array<char, MAX_STATUS_MSG_LEN> make_error_array(const char* msg)
{
    std::array<char, MAX_STATUS_MSG_LEN> arr{};
    snprintf(arr.data(), MAX_STATUS_MSG_LEN, "%s", msg);
    return arr;
}

SensorDevice::SensorDevice(const char* deviceName, const char* measurementName,
                           const char* measurementUnit, SemaphoreHandle_t commsMutex)
    : Device(deviceName),
      measurementName(measurementName),
      measurementUnit(measurementUnit),
      commsMutex(commsMutex)
{
}

Measurement SensorDevice::performMeasurement()
{
    LockGuard lockGuard(commsMutex);
    return doMeasurement();
}

Measurement SensorDevice::newValidMeasurement(const double value) const
{
    return {measurementName, value, measurementUnit, make_error_array("OK")};
}

Measurement SensorDevice::newInvalidMeasurement(const char* errorMessage) const
{
    return {measurementName, INVALID_VALUE, measurementUnit, make_error_array(errorMessage)};
}
