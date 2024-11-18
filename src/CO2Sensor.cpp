#include "CO2Sensor.h"
#include <Arduino.h>

CommunicationAttemptResult CO2Sensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}

CO2Sensor::CO2Sensor(const std::string& name, const uint8_t address)
    : I2CDevice(address), OutputDevice(name), lastCO2Value(0), isDataReady(false)
{
}

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

double CO2Sensor::readCO2Concentration()
{
    float temperature = 0.0f;
    float humidity = 0.0f;

    uint16_t error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return lastCO2Value;
    }

    if (!isDataReady) {
        Serial.println("Data not ready.");
        return lastCO2Value;
    }

    error = scd4x.readMeasurement(lastCO2Value, temperature, humidity);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return lastCO2Value;
    }

    return lastCO2Value;
}

std::map<std::string, double> CO2Sensor::readValues()
{
    return {
        {"CO2_concentration", readCO2Concentration()}
    };
}

std::vector<std::string> CO2Sensor::getMeasurableValues()
{
    return {"CO2_concentration"};
}
