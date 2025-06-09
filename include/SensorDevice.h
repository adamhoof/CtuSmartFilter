#pragma once

#include "Device.h"
#include "Measurement.h"

class SensorDevice : public Device
{
public:
    explicit SensorDevice(const char* deviceName, const char* measurementName,
                          const char* measurementUnit);

    ~SensorDevice() override = default;

    virtual Measurement performMeasurement() = 0;

protected:
    Measurement newMeasurement(double value) const;

    const char* measurementName;
    const char* measurementUnit;
};
