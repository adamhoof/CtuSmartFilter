#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include <max6675.h>

class ThermocoupleSensor : public OutputDevice, CommunicationTestable
{
public:
    CommunicationAttemptResult testCommunication() const override;

    explicit ThermocoupleSensor(const std::string& name, int8_t csPin);

    void init() override;

    std::map<std::string, double> readValues() override;

    std::vector<std::string> getMeasurableValues() override;

private:
    int8_t csPin;
    MAX6675 thermocouple;
    double readTemperature();
};
