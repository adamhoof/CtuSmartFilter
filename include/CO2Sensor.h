#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <SensirionI2CScd4x.h>
#include "I2CDevice.h"

class CO2Sensor : public I2CDevice, public OutputDevice, public CommunicationTestable
{
public:
    CommunicationAttemptResult testCommunication() const override;

    CO2Sensor(const std::string& name, uint8_t address);

    std::vector<Measurement> performMeasurements() override;

    std::vector<MeasurementName> getMeasurableValues() override;

    void init() override;

private:
    SensirionI2CScd4x scd4x;
    Measurement lastMeasurement;
    bool isDataReady;

    Measurement readCO2Concentration();
};
