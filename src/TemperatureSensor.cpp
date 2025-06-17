#include "TemperatureSensor.h"
#include <Arduino.h>
#include <InvalidValue.h>

TemperatureSensor::TemperatureSensor(const char* name, const byte address, const HTU21D& htu)
    : I2CDevice(address),
      SensorDevice(name, "room_temperature", "°C"),
      htu21d(htu)
{
}

void TemperatureSensor::init()
{
    htu21d.begin();
}

Measurement TemperatureSensor::readTemperature()
{
    const float temperature = htu21d.readTemperature();
    if (temperature == HTU21D_ERROR) {
        const char* errorMessage = "Failed to read from HTU21D chip.";
        Serial.printf("%s: %s\n", getName(), errorMessage);
        return newInvalidMeasurement(errorMessage);
    }
    return newValidMeasurement(temperature);
}

Measurement TemperatureSensor::performMeasurement()
{
    return readTemperature();
}

CommunicationAttemptResult TemperatureSensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
