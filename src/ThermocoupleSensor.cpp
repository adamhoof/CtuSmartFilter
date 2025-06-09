#include "ThermocoupleSensor.h"
#include <SPI.h>
#include <Arduino.h>
#include <sys/stat.h>
#include "InvalidValue.h"

ThermocoupleSensor::ThermocoupleSensor(const std::string& name, const int8_t csPin)
    : OutputDevice(name),
      thermocouple(csPin, &SPI),
      lastMeasurement({"filter_temperature", INVALID_VALUE, "°C"})
{
}

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
        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastMeasurement.name, INVALID_VALUE, lastMeasurement.unit};
        xSemaphoreGive(dataMutex);
        return copy;
    }

    const double temperature = thermocouple.getCelsius();

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
    const uint8_t status = thermocouple.read();
    const uint16_t rawData = thermocouple.getRawData();

    const std::string messagePrefix = "Device '" + this->name + "': ";

    if (rawData == 0x0000) {
        const std::string detail = "No response, MISO line is floating or stuck LOW.";
        return {FAILURE, messagePrefix + detail};
    }

    if (status != STATUS_OK) {
        std::string detail;

        switch (status) {
            case STATUS_NO_COMMUNICATION:
                detail = "No response, MISO line may be stuck HIGH (check power/CS pin).";
                break;
            case STATUS_ERROR:
                detail = "Responded correctly but reports an open circuit (check probe connection).";
                break;
            default:
                detail = "Reported an unknown error.";
                break;
        }
        return {FAILURE, messagePrefix + detail};
    }

    const std::string successDetail = "Responded correctly with a valid reading.";
    return {SUCCESS, messagePrefix + successDetail};
}
