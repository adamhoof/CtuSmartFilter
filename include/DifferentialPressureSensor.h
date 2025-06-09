#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <SensirionI2CSdp.h>

#include "I2CDevice.h"

class DifferentialPressureSensor : public I2CDevice, public OutputDevice, public CommunicationTestable
{
public:
    DifferentialPressureSensor(const std::string& name, byte address);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    std::vector<Measurement> performMeasurements() override;

    Measurement getDifferentialPressureValue() const;

private:
    SensirionI2CSdp differentialPressureSensor;
    Measurement lastMeasurement;

    Measurement readDifferentialPressure();
};
