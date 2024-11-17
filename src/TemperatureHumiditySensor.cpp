#include "TemperatureHumiditySensor.h"
#include <Arduino.h>

CommunicationAttemptResult TemperatureHumiditySensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}

TemperatureHumiditySensor::TemperatureHumiditySensor(const std::string& name, const byte address)
    : I2CDevice(address), OutputDevice(name)
{
}

void TemperatureHumiditySensor::init()
{
    htu21d.begin();
}

double TemperatureHumiditySensor::readTemperature()
{
    const float temperature = htu21d.readTemperature();
    if (temperature == 999) {
        Serial.println("Error reading temperature from HTU21D.");
        return -1.0;
    }
    return temperature;
}

double TemperatureHumiditySensor::readHumidity()
{
    const float humidity = htu21d.readHumidity();
    if (humidity == 999) {
        Serial.println("Error reading humidity from HTU21D.");
        return -1.0;
    }
    return humidity;
}

std::map<std::string, double> TemperatureHumiditySensor::readValues()
{
    return {
        {"temperature", readTemperature()},
        {"humidity", readHumidity()}
    };
}

std::vector<std::string> TemperatureHumiditySensor::getMeasurableValues()
{
    return {"temperature", "humidity"};
}
