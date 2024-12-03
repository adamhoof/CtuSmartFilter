#include "TemperatureHumiditySensor.h"
#include <Arduino.h>
#include <InvalidValue.h>

TemperatureHumiditySensor::TemperatureHumiditySensor(const std::string& name, const byte address)
    : I2CDevice(address),
      OutputDevice(name),
      lastTemperatureMeasurement({"room_temperature", INVALID_VALUE, "°C"}),
      lastHumidityMeasurement({"room_humidity", INVALID_VALUE, "%"}) {}

void TemperatureHumiditySensor::init() {
    htu21d.begin();
}

Measurement TemperatureHumiditySensor::readTemperature() {
    const float temperature = htu21d.readTemperature();
    if (temperature == 999) {
        Serial.println("Error reading temperature from HTU21D.");

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastTemperatureMeasurement.name, INVALID_VALUE, lastTemperatureMeasurement.unit};
        xSemaphoreGive(dataMutex);

        return copy;
    }

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    lastTemperatureMeasurement.value = temperature;
    Measurement copy = lastTemperatureMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

Measurement TemperatureHumiditySensor::readHumidity() {
    const float humidity = htu21d.readHumidity();
    if (humidity == 999) {
        Serial.println("Error reading humidity from HTU21D.");

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        Measurement copy = {lastHumidityMeasurement.name, INVALID_VALUE, lastHumidityMeasurement.unit};
        xSemaphoreGive(dataMutex);

        return copy;
    }

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    lastHumidityMeasurement.value = humidity;
    Measurement copy = lastHumidityMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

std::vector<Measurement> TemperatureHumiditySensor::performMeasurements() {
    return {readTemperature(), readHumidity()};
}

Measurement TemperatureHumiditySensor::getTemperatureValue() const {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    Measurement copy = lastTemperatureMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

Measurement TemperatureHumiditySensor::getHumidityValue() const {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    Measurement copy = lastHumidityMeasurement;
    xSemaphoreGive(dataMutex);

    return copy;
}

CommunicationAttemptResult TemperatureHumiditySensor::testCommunication() const {
    return performTemplateCommunicationTest(name, address);
}
