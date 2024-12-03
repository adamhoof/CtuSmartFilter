#pragma once

#include <CommunicationTestable.h>
#include <OutputDevice.h>
#include "I2CDevice.h"
#include <HTU21D.h>
#include <vector>
#include <string>
#include "Measurement.h"

class TemperatureHumiditySensor : public I2CDevice, public OutputDevice, public CommunicationTestable {
public:
    TemperatureHumiditySensor(const std::string& name, byte address);

    void init() override;

    CommunicationAttemptResult testCommunication() const override;

    std::vector<Measurement> performMeasurements() override;

    Measurement getTemperatureValue() const;
    Measurement getHumidityValue() const;

private:
    HTU21D htu21d;

    Measurement readTemperature();
    Measurement readHumidity();

    Measurement lastTemperatureMeasurement;
    Measurement lastHumidityMeasurement;
};
