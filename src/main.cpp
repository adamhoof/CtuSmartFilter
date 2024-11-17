#include <Arduino.h>
#include <CO2Sensor.h>
#include <DifferentialPressureSensor.h>
#include <TemperatureHumiditySensor.h>
#include <Wire.h>
#include "I2CScanner.h"

I2CScanner i2cScanner;

DifferentialPressureSensor differentialPressureSensor("DifferentialPressureSensor", 0x25);
CO2Sensor co2Sensor("CO2Sensor", 0x62);
TemperatureHumiditySensor temperatureHumiditySensor("TemperatureHumiditySensor", 0x40);

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    i2cScanner.addDevices({differentialPressureSensor, co2Sensor, temperatureHumiditySensor});

    const auto scanResults = i2cScanner.scan();
    uint8_t failuresCount = 0;

    for (const auto& result : scanResults) {
        Serial.print(result.resultStatus == I2CScanner::SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(result.message.c_str());
        if (result.resultStatus == I2CScanner::FAILURE) {
            failuresCount++;
        }
    }
    if (failuresCount > 0) {
        esp_restart();
    }
}


void loop()
{
}
