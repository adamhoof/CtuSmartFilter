#pragma once

#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include <MAX6675.h>
#include "Measurement.h"

class ThermocoupleSensor : public SensorDevice, public CommunicationTestable
{
public:
    explicit ThermocoupleSensor(const char* name, int8_t csPin);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    Measurement performMeasurement() override;

private:
    MAX6675 thermocouple;

    Measurement readTemperature();
    Measurement lastMeasurement;
};
