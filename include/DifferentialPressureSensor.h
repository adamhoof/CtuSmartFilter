#pragma once
#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include <SensirionI2CSdp.h>

#include "I2CDevice.h"

class DifferentialPressureSensor : public I2CDevice, public SensorDevice, public CommunicationTestable
{
public:
    DifferentialPressureSensor(const char* name, byte address, SemaphoreHandle_t commsMutex);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

private:
    Measurement doMeasurement() override;
    SensirionI2CSdp differentialPressureSensor;
};
