#include "CO2Sensor.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

CO2Sensor::CO2Sensor(const char* name, const uint8_t address)
    : I2CDevice(address), SensorDevice(name, "room_co2_concentration", "ppm")
{
}

void CO2Sensor::init()
{
    scd4x.begin(Wire);

    uint16_t error = scd4x.stopPeriodicMeasurement();
    if (error) {
        char errorMessage[256];
        Serial.printf("%s: Error trying to execute stopPeriodicMeasurement(): ", getName());
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    delay(20);

    error = scd4x.startPeriodicMeasurement();
    if (error) {
        char errorMessage[256];
        Serial.printf("%s: trying to execute startPeriodicMeasurement(): ", getName());
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
}

Measurement CO2Sensor::measureCO2Concentration()
{
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;

    uint16_t error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        char errorMessage[256];
        Serial.printf("%s: Error trying to execute getDataReadyFlag(): ", getName());
        errorToString(error, errorMessage, 256);

        return newInvalidMeasurement(errorMessage);
    }

    if (!isDataReady) {
        return newInvalidMeasurement("Data not ready.");
    }

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        char errorMessage[256];
        Serial.printf("%s: Error trying to execute readMeasurement(): ", getName());
        errorToString(error, errorMessage, 256);

        return newInvalidMeasurement(errorMessage);
    }

    return newValidMeasurement(co2);
}

Measurement CO2Sensor::performMeasurement()
{
    return measureCO2Concentration();
}

CommunicationAttemptResult CO2Sensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
