#include <Arduino.h>
#include "CO2Sensor.h"
#include "CommunicationTester.h"
#include "DifferentialPressureSensor.h"
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "TemperatureHumiditySensor.h"
#include <Wire.h>
#include "ThermocoupleSensor.h"
#include "secrets.h"
#include <WiFi.h>
#include "WiFiEventHandlers.h"
#include <freertos/task.h>
#include <tasks/DataCollectionTask.h>
#include <tasks/DataPublishingTask.h>
#include <tasks/FilterRegenTask.h>
#include <tasks/KeepMQTTClientAliveTask.h>
#include "MqttClient.h"

DifferentialPressureSensor differentialPressureSensor("FilterDifferentialPressureSensor", 0x25);
CO2Sensor co2Sensor("RoomCO2Sensor", 0x62);
TemperatureHumiditySensor temperatureHumiditySensor("RoomTemperatureHumiditySensor", 0x40);

ThermocoupleSensor thermocoupleSensor("FilterThermocoupleSensor", 23, 19, 18);

PWMFan pwmFan("PWMFan", 26);
PWMHeatingPad pwmHeatingPad("PWMHeatingPad", 16);

void initializeBusses()
{
    Wire.begin();
    Serial.begin(9600);
}

void initializeDevices(const std::vector<std::reference_wrapper<Device>>& devices)
{
    for (auto& device: devices) {
        device.get().init();
    }
}

void runConnectionTests(const std::vector<std::reference_wrapper<CommunicationTestable>>& devices)
{
    for (const auto& r: CommunicationTester::testDevices(devices)) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}



void setup()
{
    initializeBusses();
    initializeDevices({
        differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor, pwmFan, pwmHeatingPad
    });
    runConnectionTests({differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor});

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent(wifiDisconnectedEventHandler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(wifiConnectedEventHandler, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    configureMqttClient();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    auto filterRegenTaskParams = std::make_unique<FilterRegenTaskParams>(
        FilterRegenTaskParams{
            .co2Sensor = co2Sensor,
            .fan = pwmFan,
            .thermocoupleSensor = thermocoupleSensor,
            .heatingPad = pwmHeatingPad,
        }
    );
    xTaskCreate(filterRegenTask, "filterRegenTask", 8192, filterRegenTaskParams.release(), 2, nullptr);

    auto dataCollectionTaskParams = std::make_unique<MeasurementsPerformingTaskParams>(
        MeasurementsPerformingTaskParams{
            .devicesToCollectMeasurementsFrom = {
                differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor
            }
        }
    );
    xTaskCreate(measurementsPerformingTask, "measurementsPerformingTask", 8192, dataCollectionTaskParams.release(), 1,
                nullptr);

    auto keepMqttClientAliveTaskParams = std::make_unique<KeepMQTTClientAliveTaskParams>(
        KeepMQTTClientAliveTaskParams{
            .mqttClient = mqttClient,
        });
    xTaskCreate(keepMqttClientAlive, "keepMqttClientAlive", 5120, keepMqttClientAliveTaskParams.release(), 1, nullptr);
    while (!mqttClient.connected()) {
        delay(100);
    }

    auto dataPublishingTaskParams = std::make_unique<DataPublishingTaskParams>(
        DataPublishingTaskParams{
            .mqttClient = mqttClient,
            .devicesWhoseDataToPublish = {
                differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor
            }
        }
    );
    xTaskCreate(dataPublishingTask, "dataPublishingTask", 8192, dataPublishingTaskParams.release(), 1, nullptr);
}

void loop()
{
    delay(5000);
}
