#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>

CommunicationAttemptResult ThermocoupleSensor::testCommunication() const
{
    Serial.println("ThermocoupleSensor test");
}

ThermocoupleSensor::ThermocoupleSensor(const std::string& name, const int8_t csPin)
    : OutputDevice(name), csPin(csPin), thermocouple(SCK, csPin, MISO) {}

void ThermocoupleSensor::init() {
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    Serial.println("ThermocoupleSensor initialized.");
    delay(500);
}

double ThermocoupleSensor::readTemperature() {
    double temperature = thermocouple.readCelsius();
    if (temperature == NAN || temperature < -100.0) {
        Serial.println("Error reading temperature from MAX6675.");
        return -1.0;
    }
    return temperature;
}

std::map<std::string, double> ThermocoupleSensor::readValues() {
    return {
            {"temperature", readTemperature()}
    };
}

std::vector<std::string> ThermocoupleSensor::getMeasurableValues() {
    return {"temperature"};
}
