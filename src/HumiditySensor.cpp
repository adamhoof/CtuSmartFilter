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
    if (humidity == 999) {
        Serial.println("Error reading humidity from HTU21D.");

        return newMeasurement(INVALID_VALUE);
    }

    return newMeasurement(humidity);
}

Measurement HumiditySensor::performMeasurement()
{
    return readHumidity();
}

CommunicationAttemptResult HumiditySensor::testCommunication()
{
    return performTemplateCommunicationTest(getName(), address);
}
