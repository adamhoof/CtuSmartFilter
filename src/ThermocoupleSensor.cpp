#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>
#include "InvalidValue.h"

ThermocoupleSensor::ThermocoupleSensor(const std::string& name, const int8_t csPin, const int8_t misoPin,
                                       const int8_t sckPin)
    : OutputDevice(name),
      csPin(csPin),
      misoPin(misoPin),
      sckPin(sckPin),
      thermocouple(sckPin, csPin, misoPin),
      lastMeasurement({"filter_temperature", INVALID_VALUE, "°C"})
{
}

void ThermocoupleSensor::init()
{
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    delay(100);
    Serial.println("ThermocoupleSensor initialized.");
}

Measurement ThermocoupleSensor::readTemperature()
{
    const double temperature = thermocouple.readCelsius();

    if (isnan(temperature) || temperature < -100.0) {
        Serial.println("Error reading temperature from MAX6675.");

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastMeasurement.name, INVALID_VALUE, lastMeasurement.unit};
        xSemaphoreGive(dataMutex);

        return copy;
    }

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    lastMeasurement.value = temperature;
    Measurement copy = lastMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

std::vector<Measurement> ThermocoupleSensor::performMeasurements()
{
    return {readTemperature()};
}

Measurement ThermocoupleSensor::getTemperatureValue() const
{
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    Measurement copy = lastMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

CommunicationAttemptResult ThermocoupleSensor::testCommunication()
{
    Serial.println("ThermocoupleSensor test");

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csPin, LOW);

    uint8_t response = SPI.transfer(0x00);

    digitalWrite(csPin, HIGH);
    SPI.endTransaction();

    if (response == 0 || response == 0xFF) {
        return {FAILURE, "ThermocoupleSensor: No response or invalid data from SPI device"};
    } else {
        return {SUCCESS, "ThermocoupleSensor: Device responded correctly via SPI"};
    }
}
