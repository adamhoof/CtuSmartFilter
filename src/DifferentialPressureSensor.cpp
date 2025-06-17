#include "DifferentialPressureSensor.h"
#include <Wire.h>
#include <Arduino.h>
#include <InvalidValue.h>

DifferentialPressureSensor::DifferentialPressureSensor(const char* name, const uint8_t address)
    : I2CDevice(address),
      SensorDevice(name, "filter_differential_pressure", "Pa"){}

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

Measurement DifferentialPressureSensor::readDifferentialPressure() {
    float differentialPressure = 0.0f;
    float temperature = 0.0f;

    const uint16_t error = differentialPressureSensor.readMeasurement(differentialPressure, temperature);

    if (error) {
        char errorMessageBuffer[256];
        errorToString(error, errorMessageBuffer, sizeof(errorMessageBuffer));
        Serial.printf("%s: %s\n", getName(), errorMessageBuffer);
        return newInvalidMeasurement(errorMessageBuffer);
    }
    return newValidMeasurement(differentialPressure);
}

Measurement DifferentialPressureSensor::performMeasurement()
{
    return readDifferentialPressure();
}

CommunicationAttemptResult DifferentialPressureSensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
