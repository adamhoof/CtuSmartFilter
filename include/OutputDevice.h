#pragma once
#include <Device.h>
#include <string>
#include <vector>

using MeasurementName = std::string;
using Unit = std::string;

struct Measurement {

    MeasurementName name;
    double value;
    Unit unit;
};

class OutputDevice : public Device
{
public:
    explicit OutputDevice(std::string deviceName);

    ~OutputDevice() override = default;

    virtual std::vector<Measurement> performMeasurements() = 0;

    virtual std::vector<MeasurementName> getMeasurableValues() = 0;

    std::string getName() const;

protected:
    std::string name;
};
