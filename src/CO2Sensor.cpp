#include "CO2Sensor.h"
#include <Arduino.h>
#include <InvalidValue.h>
#include <freertos/FreeRTOS.h>

CO2Sensor::CO2Sensor(const char* name, const uint8_t address)
    : I2CDevice(address), SensorDevice(name, "room_co2_concentration", "ppm"),
      lastValue(INVALID_VALUE), isDataReady(false) {}

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
    delay(20);

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

        return newMeasurement(INVALID_VALUE);
    }

    if (!isDataReady) {
        Serial.println("Data not ready.");
        return newMeasurement(lastValue);
    }

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);

        return newMeasurement(INVALID_VALUE);
    }

    return newMeasurement(co2);
}

Measurement CO2Sensor::performMeasurement()
{
    return {measureCO2Concentration()};
}

CommunicationAttemptResult CO2Sensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
