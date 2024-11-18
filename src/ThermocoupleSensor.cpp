#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>

CommunicationAttemptResult ThermocoupleSensor::testCommunication() const
{
    Serial.println("ThermocoupleSensor test");
}

ThermocoupleSensor::ThermocoupleSensor(const std::string& name, const int8_t csPin)
    : OutputDevice(name), csPin(csPin), thermocouple(SCK, csPin, MISO), lastMeasurement()
{
}

void ThermocoupleSensor::init()
{
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    Serial.println("ThermocoupleSensor initialized.");
    delay(500);
}

Measurement ThermocoupleSensor::readTemperature()
{
    const double temperature = thermocouple.readCelsius();
    if (temperature == NAN || temperature < -100.0) {
        Serial.println("Error reading temperature from MAX6675.");
        return {lastMeasurement.name, -1, lastMeasurement.unit};
    }
    lastMeasurement.value = temperature;
    return lastMeasurement;
}

std::vector<Measurement> ThermocoupleSensor::performMeasurements()
{
    return {readTemperature()};
}

std::vector<std::string> ThermocoupleSensor::getMeasurableValues()
{
    return {"temperature"};
}
