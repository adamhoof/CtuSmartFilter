#pragma once
#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include <MAX6675.h>
#include "Measurement.h"

class ThermocoupleSensor : public SensorDevice, public CommunicationTestable
{
public:
    explicit ThermocoupleSensor(const char* name, int8_t csPin, uint64_t measurementRefreshMS, SemaphoreHandle_t commsMutex);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

private:
    Measurement doMeasurement() override;
    MAX6675 thermocouple;
};
