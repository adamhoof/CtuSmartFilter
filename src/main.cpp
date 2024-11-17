#include <Arduino.h>
#include <CO2Sensor.h>
#include <CommunicationTester.h>
#include <DifferentialPressureSensor.h>
#include <PWMFan.h>
#include <PWMHeatingPad.h>
#include <SPI.h>
#include <TemperatureHumiditySensor.h>
#include <Wire.h>
#include <ThermocoupleSensor.h>

DifferentialPressureSensor differentialPressureSensor("DifferentialPressureSensor", 0x25);
CO2Sensor co2Sensor("CO2Sensor", 0x62);
TemperatureHumiditySensor temperatureHumiditySensor("TemperatureHumiditySensor", 0x40);

constexpr int SPI_CS = 17;
ThermocoupleSensor thermocoupleSensor("ThermocoupleSensor", SPI_CS);

PWMFan pwmFan("PWMFan", 26,1);
PWMHeatingPad pwmHeatingPad("PWMHeatingPad", 16,2);

void setup()
{
    Wire.begin();
    SPI.begin(SCK, MISO, MOSI, SPI_CS);
    Serial.begin(115200);

    const std::vector<std::reference_wrapper<Device>> devices {differentialPressureSensor,co2Sensor,temperatureHumiditySensor,thermocoupleSensor, pwmFan, pwmHeatingPad};
    for (auto device : devices) {
        device.get().init();
    }

    CommunicationTester::testDevices({differentialPressureSensor, co2Sensor, temperatureHumiditySensor});

    pwmFan.init();
    pwmFan.setPower(255);
    delay(3000);
    pwmFan.setPower(0);

}

void loop()
{

}
