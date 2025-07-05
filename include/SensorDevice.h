#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "CommunicationTestable.h"
#include "Device.h"
#include "Measurement.h"

class SensorDevice : public Device
{
public:
    explicit SensorDevice(const char* deviceName, const char* measurementName,
                          const char* measurementUnit, SemaphoreHandle_t commsMutex);

    ~SensorDevice() override = default;

    Measurement performMeasurement();
    virtual CommunicationAttemptResult testCommunication() = 0;

protected:
    virtual Measurement doMeasurement() = 0;

    Measurement newValidMeasurement(double value) const;
    Measurement newInvalidMeasurement(const char* errorMessage) const;

    const char* measurementName;
    const char* measurementUnit;
    SemaphoreHandle_t commsMutex;
};
