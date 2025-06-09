#include <Arduino.h>
#include <SPI.h>

#include "CO2Sensor.h"
#include "CommunicationTester.h"
#include "DifferentialPressureSensor.h"
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "TemperatureSensor.h"
#include <Wire.h>
#include "ThermocoupleSensor.h"
#include "secrets.h"
#include <WiFi.h>
#include "WiFiEventHandlers.h"
#include <freertos/task.h>
#include <tasks/DataCollectionTask.h>
#include <tasks/DataPublishingTask.h>
#include <tasks/FilterRegenTask.h>
#include <tasks/KeepConnectionsAliveTask.h>

#include "HumiditySensor.h"
#include "MqttClientWrapper.h"

DifferentialPressureSensor differentialPressureSensor("FilterDifferentialPressureSensor", 0x25);
CO2Sensor co2Sensor("RoomCO2Sensor", 0x62);

// To keep things simple, the multi value HTU21D sensor is split into 2 different sensors, that both use the underlying
HTU21D htu21d;
TemperatureSensor temperatureSensor("RoomTemperatureSensor", 0x40, htu21d);
HumiditySensor humiditySensor("RoomHumiditySensor", 0x40, htu21d);

constexpr int8_t csPin = 23, sckPin = 18, misoPin = 19;
ThermocoupleSensor thermocoupleSensor("FilterThermocoupleSensor", csPin);

PWMFan pwmFan("PWMFan", 26);
PWMHeatingPad pwmHeatingPad("PWMHeatingPad", 16);

void initializeBusses()
{
    Wire.begin();
    Serial.begin(9600);
    // MAX6675 is output only, no mosi needed
    SPI.begin(sckPin, misoPin);
}

void initializeDevices(const std::vector<Device*>& devices)
{
    for (auto& device: devices) {
        device->init();
    }
}

void runConnectionTests(const std::vector<CommunicationTestable*>& devices)
{
    for (const auto& r: CommunicationTester::testDevices(devices)) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}

void setup()
{
    // avoid dangling mosi pin state when disconnected
    pinMode(misoPin, INPUT_PULLUP);
    initializeBusses();
    initializeDevices({
        /*&differentialPressureSensor,
         *&co2Sensor,
         *&temperatureHumiditySensor,*/
        &thermocoupleSensor /*,
        &pwmFan,
        &pwmHeatingPad*/
    });

    runConnectionTests({
        /*&differentialPressureSensor,
         *&co2Sensor,
         *&temperatureHumiditySensor,*/
        &thermocoupleSensor
    });

    /*WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent(wifiDisconnectedEventHandler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(wifiConnectedEventHandler, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    configureMqttClient();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);*/

    auto measurementsPerformingTaskParams = new MeasurementsPerformingTaskParams{
        .sensorsToCollectMeasurementsFrom = {
            /*differentialPressureSensor,
            co2Sensor,
            temperatureHumiditySensor,*/
            &thermocoupleSensor
        }
    };

    xTaskCreate(measurementsPerformingTask, "measurementsPerformingTask", 8192, measurementsPerformingTaskParams, 1,
                nullptr);

    /*auto filterRegenTaskParams = new FilterRegenTaskParams{
            .co2Sensor = co2Sensor,
            .fan = pwmFan,
            .heatingPad = pwmHeatingPad,
            .thermocoupleSensor = thermocoupleSensor,
            nullptr
        };
    xTaskCreate(filterRegenTask, "filterRegenTask", 8192, filterRegenTaskParams, 2, nullptr);*/

    /*auto keepConnectionsAliveTaskParams = new KeepConnectionsAliveTaskParams{
            .mqttClient = mqttClient,
        };
        xTaskCreate(keepConnectionsAlive, "keepConnectionsAlive", 10000, keepConnectionsAliveTaskParams, 1, nullptr);*/

    /*auto dataPublishingTaskParams = new DataPublishingTaskParams{
            .mqttClient = mqttClient,
            .devicesWhoseDataToPublish = {
                &differentialPressureSensor, &co2Sensor, &temperatureHumiditySensor, &thermocoupleSensor
            }
        };

    xTaskCreate(dataPublishingTask, "dataPublishingTask", 8192, dataPublishingTaskParams, 1, nullptr);*/
}

void loop()
{
    delay(5000);
}
