#pragma once
#include <Device.h>
#include <map>
#include <string>
#include <vector>

class OutputDevice : public Device
{
public:
    explicit OutputDevice(std::string deviceName);

    ~OutputDevice() override = default;

    virtual std::map<std::string, double> readValues() = 0;

    virtual std::vector<std::string> getMeasurableValues() = 0;

    std::string getName() const;

protected:
    std::string name;
};
