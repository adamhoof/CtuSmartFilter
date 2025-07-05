#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>

ThermocoupleSensor::ThermocoupleSensor(const char* name, const int8_t csPin, SemaphoreHandle_t commsMutex)
    : SensorDevice(name, "filter_temperature", "°C", commsMutex),
      thermocouple(csPin, &SPI){}

void ThermocoupleSensor::init()
{
    thermocouple.begin();
}

Measurement ThermocoupleSensor::doMeasurement()
{
    const uint8_t status = thermocouple.read();
    const uint16_t rawData = thermocouple.getRawData();

    if (rawData == 0x0000) {
        return newInvalidMeasurement("No response, MISO line is floating or stuck LOW.");
    }

    if (status != STATUS_OK) {
        const char* errorMessage = "Unknown error";

        if (status == STATUS_ERROR) {
            errorMessage = "Thermocouple is open-circuit or disconnected.";
        } else if (status == STATUS_NO_COMMUNICATION) {
            errorMessage = "No communication with MAX6675 chip.";
        }
        Serial.printf("%s: %s\n", getName(), errorMessage);
        return newInvalidMeasurement(errorMessage);
    }

    const double temperature = thermocouple.getCelsius();

    return newValidMeasurement(temperature);
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
