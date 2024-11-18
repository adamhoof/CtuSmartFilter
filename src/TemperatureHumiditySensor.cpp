#include "TemperatureHumiditySensor.h"
#include <Arduino.h>

CommunicationAttemptResult TemperatureHumiditySensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}

TemperatureHumiditySensor::TemperatureHumiditySensor(const std::string& name, const byte address)
    : I2CDevice(address), OutputDevice(name), lastTemperatureMeasurement({"temperature", -1.0, "°C"}),
      lastHumidityMeasurement({"humidity", -1.0, "%"})
{}

void TemperatureHumiditySensor::init()
{
    htu21d.begin();
}

Measurement TemperatureHumiditySensor::readTemperature()
{
    const float temperature = htu21d.readTemperature();
    if (temperature == 999) {
        Serial.println("Error reading temperature from HTU21D.");
        return {lastTemperatureMeasurement.name, -1, lastTemperatureMeasurement.unit};
    }

    lastTemperatureMeasurement.value = temperature;
    return lastTemperatureMeasurement;
}

Measurement TemperatureHumiditySensor::readHumidity()
{
    const float humidity = htu21d.readHumidity();
    if (humidity == 999) {
        Serial.println("Error reading humidity from HTU21D.");
        return {lastHumidityMeasurement.name, -1, lastHumidityMeasurement.unit};
    }

    lastHumidityMeasurement.value = humidity;
    return lastHumidityMeasurement;
}

std::vector<Measurement> TemperatureHumiditySensor::performMeasurements()
{
    return {readTemperature(), readHumidity()};
}

std::vector<std::string> TemperatureHumiditySensor::getMeasurableValues()
{
    return {"temperature", "humidity"};
}
