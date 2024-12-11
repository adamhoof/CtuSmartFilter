#include "CO2Sensor.h"
#include <Arduino.h>
#include <InvalidValue.h>
#include <freertos/FreeRTOS.h>

CO2Sensor::CO2Sensor(const std::string& name, const uint8_t address)
    : I2CDevice(address), OutputDevice(name), lastCO2Measurement({"room_co2_concentration", INVALID_VALUE, "ppm"}),
      isDataReady(false) {}

void CO2Sensor::init()
{
    scd4x.begin(Wire);

    uint16_t error = scd4x.stopPeriodicMeasurement();
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    error = scd4x.startPeriodicMeasurement();
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
}

Measurement CO2Sensor::measureCO2Concentration()
{
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;

    uint16_t error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastCO2Measurement.name, INVALID_VALUE, lastCO2Measurement.unit};
        xSemaphoreGive(dataMutex);
        return copy;
    }

    if (!isDataReady) {
        Serial.println("Data not ready.");

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = lastCO2Measurement;
        xSemaphoreGive(dataMutex);
        return copy;
    }

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastCO2Measurement.name, INVALID_VALUE, lastCO2Measurement.unit};
        xSemaphoreGive(dataMutex);
        return copy;
    }

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    lastCO2Measurement.value = co2;
    Measurement copy = lastCO2Measurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

std::vector<Measurement> CO2Sensor::performMeasurements()
{
    return {measureCO2Concentration()};
}

Measurement CO2Sensor::getCO2Value() const
{
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    Measurement copy = lastCO2Measurement;
    xSemaphoreGive(dataMutex);
    return copy;
}

CommunicationAttemptResult CO2Sensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}
