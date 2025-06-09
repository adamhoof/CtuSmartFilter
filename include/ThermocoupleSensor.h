#pragma once

#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <max6675.h>
#include <vector>
#include <string>
#include "Measurement.h"

class ThermocoupleSensor : public OutputDevice, public CommunicationTestable
{
public:
    explicit ThermocoupleSensor(const std::string& name, int8_t csPin, int8_t misoPin, int8_t sckPin);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

    std::vector<Measurement> performMeasurements() override;

    Measurement getTemperatureValue() const;

private:
    int8_t csPin;
    int8_t misoPin;
    int8_t sckPin;
    MAX6675 thermocouple;

    Measurement readTemperature();
    Measurement lastMeasurement;
};
