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

    std::map<std::string, double> readValues() override;

    std::vector<std::string> getMeasurableValues() override;

    void init() override;

private:
    SensirionI2CScd4x scd4x;
    uint16_t lastCO2Value;
    bool isDataReady;

    double readCO2Concentration();
};
