#include "DifferentialPressureSensor.h"
#include <Wire.h>
#include <Arduino.h>
#include <InvalidValue.h>

CommunicationAttemptResult DifferentialPressureSensor::testCommunication() const
{
    return performTemplateCommunicationTest(name, address);
}

DifferentialPressureSensor::DifferentialPressureSensor(const std::string& name, const byte address)
    : I2CDevice(address), OutputDevice(name), lastMeasurement({"pressure", INVALID_VALUE, "Pa"})
{
}

void DifferentialPressureSensor::init()
{
    differentialPressureSensor.begin(Wire, address);

    char errorMessage[256];

    uint16_t error = differentialPressureSensor.stopContinuousMeasurement();
    if (error) {
        Serial.print("Error trying to execute stopContinuousMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    error = differentialPressureSensor.startContinuousMeasurementWithDiffPressureTCompAndAveraging();
    if (error) {
        Serial.print("Error trying to execute startContinuousMeasurementWithDiffPressureTCompAndAveraging(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
}

Measurement DifferentialPressureSensor::readDifferentialPressure()
{
    float differentialPressure = 0.0f;
    float temperature = 0.0f;

    const uint16_t error = differentialPressureSensor.readMeasurement(differentialPressure, temperature);
    if (error) {
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return {lastMeasurement.name, INVALID_VALUE, lastMeasurement.unit};
    }

    lastMeasurement.value = static_cast<double>(differentialPressure);
    return lastMeasurement;
}

std::vector<Measurement> DifferentialPressureSensor::performMeasurements()
{
    return {readDifferentialPressure()};
}

std::vector<std::string> DifferentialPressureSensor::getMeasurableValues()
{
    return {"pressure"};
}
