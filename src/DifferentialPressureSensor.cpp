#include "DifferentialPressureSensor.h"
#include <Wire.h>
#include <Arduino.h>
#include <InvalidValue.h>

DifferentialPressureSensor::DifferentialPressureSensor(const std::string& name, const uint8_t address)
    : I2CDevice(address),
      OutputDevice(name),
      lastMeasurement({"filter_differential_pressure", INVALID_VALUE, "Pa"}) {}

void DifferentialPressureSensor::init() {
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
        char errorMessage[256];
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastMeasurement.name, INVALID_VALUE, lastMeasurement.unit};
        xSemaphoreGive(dataMutex);

        return copy;
    }

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    lastMeasurement.value = static_cast<double>(differentialPressure);
    Measurement copy = lastMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

std::vector<Measurement> DifferentialPressureSensor::performMeasurements() {
    return {readDifferentialPressure()};
}

Measurement DifferentialPressureSensor::getDifferentialPressureValue() const {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    Measurement copy = lastMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

CommunicationAttemptResult DifferentialPressureSensor::testCommunication() {
    return performTemplateCommunicationTest(name, address);
}
