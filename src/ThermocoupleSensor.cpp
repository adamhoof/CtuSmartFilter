#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>
#include <sys/stat.h>
#include "InvalidValue.h"

ThermocoupleSensor::ThermocoupleSensor(const char* name, const int8_t csPin)
    : SensorDevice(name, "filter_temperature", "°C"),
      thermocouple(csPin, &SPI){}

void ThermocoupleSensor::init()
{
    thermocouple.begin();
    Serial.println("ThermocoupleSensor initialized.");
}

Measurement ThermocoupleSensor::readTemperature()
{
    const uint8_t status = thermocouple.read();

    if (status != STATUS_OK) {
        if (status == STATUS_ERROR) {
            Serial.println("Error: Thermocouple is open-circuit or disconnected.");
        }
        else if (status == STATUS_NO_COMMUNICATION) {
            Serial.println("Error: No communication with MAX6675 chip.");
        } else {
            Serial.println("Error: Unknown error.");
        }
        return newMeasurement(INVALID_VALUE);
    }

    const double temperature = thermocouple.getCelsius();

    return newMeasurement(temperature);
}

Measurement ThermocoupleSensor::performMeasurement()
{
    return {readTemperature()};
}

CommunicationAttemptResult ThermocoupleSensor::testCommunication() {
    static char messageBuffer[128];
    const uint8_t status = thermocouple.read();
    const uint16_t rawData = thermocouple.getRawData();
    const char* errorDetail = nullptr;

    if (rawData == 0x0000) {
        errorDetail = "No response, MISO line is floating or stuck LOW.";
    } else if (status == STATUS_NO_COMMUNICATION) {
        errorDetail = "No response, MISO line may be stuck HIGH (check power/CS pin).";
    } else if (status == STATUS_ERROR) {
        errorDetail = "Responded correctly but reports an open circuit (check probe).";
    }

    if (errorDetail != nullptr) {
        snprintf(messageBuffer, sizeof(messageBuffer), "Device '%s': %s",
                 getName(), errorDetail);
        return {FAILURE, messageBuffer};
    }

    snprintf(messageBuffer, sizeof(messageBuffer),
             "Device '%s': Responded correctly with a valid reading.", getName());
    return {SUCCESS, messageBuffer};
}
