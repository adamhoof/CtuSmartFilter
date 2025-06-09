#include "SensorDevice.h"

SensorDevice::SensorDevice(const char* deviceName, const char* measurementName,
                           const char* measurementUnit)
    : Device(deviceName),
      measurementName(measurementName),
      measurementUnit(measurementUnit) {
}

Measurement SensorDevice::newMeasurement(const double value) const {
    return {measurementName, value, measurementUnit};
}