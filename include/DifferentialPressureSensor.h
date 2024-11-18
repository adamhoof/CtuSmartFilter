#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <SensirionI2CSdp.h>

#include "I2CDevice.h"

class DifferentialPressureSensor : public I2CDevice, public OutputDevice, public CommunicationTestable
{
public:
    CommunicationAttemptResult testCommunication() const override;

    DifferentialPressureSensor(const std::string& name, byte address);

    std::vector<Measurement> performMeasurements() override;

    std::vector<MeasurementName> getMeasurableValues() override;

    void init() override;

private:
    SensirionI2CSdp differentialPressureSensor;
    Measurement lastMeasurement;

    Measurement readDifferentialPressure();
};
