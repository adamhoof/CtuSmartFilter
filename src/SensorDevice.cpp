#include "SensorDevice.h"
#include <cstring>
#include <HardwareSerial.h>
#include "InvalidValue.h"
#include "LockGuard.h"

inline std::array<char, MAX_STATUS_MSG_LEN> make_error_array(const char* msg)
{
    std::array<char, MAX_STATUS_MSG_LEN> arr{};
    snprintf(arr.data(), MAX_STATUS_MSG_LEN, "%s", msg);
    return arr;
}

SensorDevice::SensorDevice(const char* deviceName, const char* measurementName,
                           const char* measurementUnit, const uint64_t measurementRefreshMS,
                           const SemaphoreHandle_t commsMutex)
    : Device(deviceName),
      measurementName(measurementName),
      measurementUnit(measurementUnit),
      commsMutex(commsMutex),
      valueMutex(xSemaphoreCreateMutex()),
      measurementRefresh(pdMS_TO_TICKS(measurementRefreshMS))
{
}

SensorDevice::~SensorDevice()
{
    vSemaphoreDelete(valueMutex);
}

Measurement SensorDevice::performMeasurement()
{
    LockGuard valueGuard(valueMutex);
    if (xTaskGetTickCount() - lastMeasurementTime < measurementRefresh) {
        return lastMeasurement;
    }

    Measurement measurement; {
        LockGuard commsGuard(commsMutex);
        measurement = doMeasurement();
    }

    lastMeasurementTime = xTaskGetTickCount();
    lastMeasurement = measurement;

    if (strcmp(measurement.statusMessage.data(), "OK") != 0) {
        if (++consecutiveMeasurementFailures > MAX_CONSECUTIVE_FAILURES) {
            reliabilityStatus = ReliabilityStatus::UNRELIABLE;
        }
    }
    else { consecutiveMeasurementFailures = 0; }

    return measurement;
}

bool SensorDevice::isReliable() const
{
    LockGuard lockGuard(valueMutex);
    return reliabilityStatus == ReliabilityStatus::RELIABLE;
}

void SensorDevice::updateMeasurementRefreshMS(const uint64_t measurementRefreshMS)
{
    LockGuard lockGuard(valueMutex);
    measurementRefresh = pdMS_TO_TICKS(measurementRefreshMS);
}

uint64_t SensorDevice::getMeasurementRefreshMS() const
{
    LockGuard lockGuard(valueMutex);
    return pdTICKS_TO_MS(measurementRefresh);
}

Measurement SensorDevice::newValidMeasurement(const float value) const
{
    return {measurementName, value, measurementUnit, make_error_array("OK")};
}

Measurement SensorDevice::newInvalidMeasurement(const char* errorMessage) const
{
    return {measurementName, INVALID_VALUE, measurementUnit, make_error_array(errorMessage)};
}
