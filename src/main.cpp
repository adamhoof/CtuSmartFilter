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
  i2cScanner.addDevices({differentialPressureSensor,co2Sensor,temperatureHumiditySensor});
}


void loop()
{
  i2cScanner.scan();
}
