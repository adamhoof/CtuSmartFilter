#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <SensirionI2CScd4x.h>
#include "I2CDevice.h"

class CO2Sensor : public I2CDevice, public OutputDevice, public CommunicationTestable
{
public:
    CO2Sensor(const std::string& name, uint8_t address);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    std::vector<Measurement> performMeasurements() override;

    Measurement getCO2Value() const;

private:
    SensirionI2CScd4x scd4x;
    Measurement lastCO2Measurement;
    bool isDataReady;

    Measurement measureCO2Concentration();
};
