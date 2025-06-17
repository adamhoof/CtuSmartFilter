#include "HumiditySensor.h"
#include <Arduino.h>
#include <InvalidValue.h>

HumiditySensor::HumiditySensor(const char* name, const byte address, const HTU21D& htu)
    : I2CDevice(address),
      SensorDevice(name, "room_humidity", "%"),
      htu21d(htu)
{
}

void HumiditySensor::init()
{
    htu21d.begin();
}

Measurement HumiditySensor::readHumidity()
{
    const float humidity = htu21d.readHumidity();
    if (humidity == HTU21D_ERROR) {
        const char* errorMessage = "Failed to read from HTU21D chip.";
        Serial.printf("%s: %s\n", getName(), errorMessage);
        return newInvalidMeasurement(errorMessage);
    }
    return newValidMeasurement(humidity);
}

Measurement HumiditySensor::performMeasurement()
{
    return readHumidity();
}

CommunicationAttemptResult HumiditySensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
