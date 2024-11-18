#include "CO2Sensor.h"
#include <Arduino.h>

CommunicationAttemptResult CO2Sensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}

CO2Sensor::CO2Sensor(const std::string& name, const uint8_t address)
    : I2CDevice(address), OutputDevice(name), lastMeasurement({"CO2_concentration",-1, "ppm"}), isDataReady(false)
{}

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

Measurement CO2Sensor::readCO2Concentration()
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
        return {lastMeasurement.name, -1, lastMeasurement.unit};
    }

    if (!isDataReady) {
        Serial.println("Data not ready.");
        return lastMeasurement;
    }

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return {lastMeasurement.name, -1, lastMeasurement.unit};
    }

    lastMeasurement.value = static_cast<double>(co2);
    return lastMeasurement;
}


std::vector<Measurement> CO2Sensor::performMeasurements()
{
    return {readCO2Concentration()};
}

std::vector<std::string> CO2Sensor::getMeasurableValues()
{
    return {"CO2_concentration"};
}
