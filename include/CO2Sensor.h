#pragma once
#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include <SensirionI2CScd4x.h>
#include "I2CDevice.h"

class CO2Sensor : public I2CDevice, public SensorDevice, public CommunicationTestable
{
public:
    CO2Sensor(const char* name, uint8_t address);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    Measurement performMeasurement() override;

private:
    SensirionI2CScd4x scd4x;

    Measurement measureCO2Concentration();
};
