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
    if (temperature == 999) {
        Serial.println("Error reading temperature from HTU21D.");

        return newMeasurement(INVALID_VALUE);
    }

    return newMeasurement(temperature);
}

Measurement TemperatureSensor::performMeasurement()
{
    return readTemperature();
}

CommunicationAttemptResult TemperatureSensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
