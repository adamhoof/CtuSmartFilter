#pragma once
#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include "I2CDevice.h"
#include <HTU21D.h>

class TemperatureHumiditySensor : public I2CDevice, public OutputDevice, public CommunicationTestable
{
public:
    CommunicationAttemptResult testCommunication() const override;

    TemperatureHumiditySensor(const std::string& name, byte address);

    std::vector<Measurement> performMeasurements() override;

    std::vector<MeasurementName> getMeasurableValues() override;

    void init() override;

private:
    HTU21D htu21d;

    Measurement readTemperature();
    Measurement lastTemperatureMeasurement;

    Measurement readHumidity();
    Measurement lastHumidityMeasurement;
};
