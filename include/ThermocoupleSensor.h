#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <max6675.h>

class ThermocoupleSensor : public OutputDevice, public CommunicationTestable
{
public:
    CommunicationAttemptResult testCommunication() const override;

    explicit ThermocoupleSensor(const std::string& name, int8_t csPin, int8_t misoPin, int8_t sckPin);

    void init() override;

    std::vector<Measurement> performMeasurements() override;

    std::vector<MeasurementName> getMeasurableValues() override;

private:
    int8_t csPin;
    int8_t misoPin;
    int8_t sckPin;
    MAX6675 thermocouple;

    Measurement readTemperature();
    Measurement lastMeasurement;
};
