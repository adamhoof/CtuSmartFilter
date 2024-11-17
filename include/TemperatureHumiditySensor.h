#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include "I2CDevice.h"
#include <SparkFunHTU21D.h>

class TemperatureHumiditySensor : public I2CDevice, public OutputDevice, public CommunicationTestable {
public:
    CommunicationAttemptResult testCommunication() const override;

    TemperatureHumiditySensor(const std::string& name, byte address);

    std::map<std::string, double> readValues() override;
    std::vector<std::string> getMeasurableValues() override;
    void init() override;

private:
    HTU21D htu21d;

    double readTemperature();
    double readHumidity();
};
