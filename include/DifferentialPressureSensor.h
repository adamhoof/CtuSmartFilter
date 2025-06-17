#pragma once
#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include <SensirionI2CSdp.h>

#include "I2CDevice.h"

class DifferentialPressureSensor : public I2CDevice, public SensorDevice, public CommunicationTestable
{
public:
    DifferentialPressureSensor(const char* name, byte address);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    Measurement performMeasurement() override;

    Measurement getDifferentialPressureValue() const;

private:
    SensirionI2CSdp differentialPressureSensor;

    Measurement readDifferentialPressure();
};
