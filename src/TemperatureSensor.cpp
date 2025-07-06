#include "TemperatureSensor.h"
#include <Arduino.h>

TemperatureSensor::TemperatureSensor(const char* name, const byte address, HTU21D& htu, const uint64_t measurementRefreshMS, const SemaphoreHandle_t commsMutex)
    : I2CDevice(address),
      SensorDevice(name, "room_temperature", "°C", measurementRefreshMS, commsMutex),
      htu21d(htu)
{
}

void TemperatureSensor::init()
{
    htu21d.begin();
}

Measurement TemperatureSensor::doMeasurement()
{
    const float temperature = htu21d.readTemperature();
    if (temperature == HTU21D_ERROR) {
        const char* errorMessage = "Failed to read from HTU21D chip.";
        Serial.printf("%s: %s\n", getName(), errorMessage);
        return newInvalidMeasurement(errorMessage);
    }
    return newValidMeasurement(temperature);
}

CommunicationAttemptResult TemperatureSensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
