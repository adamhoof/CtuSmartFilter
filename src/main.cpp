#include <Arduino.h>
#include "CO2Sensor.h"
#include "CommunicationTester.h"
#include "DifferentialPressureSensor.h"
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "TemperatureHumiditySensor.h"
#include <Wire.h>
#include "ThermocoupleSensor.h"
#include <ArduinoJson.h>
#include "secrets.h"
#include <WiFi.h>
#include "WiFiEventHandlers.h"
#include <freertos/task.h>
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
    const auto results = CommunicationTester::testDevices(devices);
    for (const auto& r: results) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}

struct DeviceMeasurements
{
    std::string deviceName;
    std::vector<Measurement> measurements;
};

using CollectedData = std::vector<DeviceMeasurements>;

void collectData(const std::vector<std::reference_wrapper<OutputDevice>>& devices, CollectedData& collectedData)
{
    for (const auto& device: devices) {
        collectedData.push_back({device.get().getName(), device.get().performMeasurements()});
        delay(50);
    }
}

void serializeToJson(const CollectedData& collectedData, char* buffer, size_t bufferSize)
{
    JsonDocument doc;

    for (const auto& deviceData: collectedData) {
        for (const auto& measurement: deviceData.measurements) {
            JsonObject measurementObject = doc[measurement.name.c_str()].to<JsonObject>();
            measurementObject["value"] = measurement.value;
            measurementObject["unit"] = measurement.unit.c_str();
        }
    }

    serializeJson(doc, buffer, bufferSize);
}

void dataCollectionTask(void* parameter)
{
    const std::vector<std::reference_wrapper<OutputDevice>> devices = {
        differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor
    };

    constexpr size_t bufferSize = 1024;
    char jsonBuffer[bufferSize];
    while (true) {
        CollectedData collectedData;
        collectData(devices, collectedData);
        serializeToJson(collectedData, jsonBuffer, bufferSize);

        mqttClient.publish(DATA_TOPIC, 1, false, jsonBuffer);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void configureMqttClient(espMqttClientSecure& mqttClient)
{
    mqttClient.setCACert(rootCAChain);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCleanSession(true);
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

    configureMqttClient(mqttClient);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    xTaskCreate(keepMqttClientAlive, "keepMqttAlive", 5120, nullptr, 1, nullptr);
    while (!mqttClient.connected()) {
        delay(100);
    }
    xTaskCreate(dataCollectionTask, "DataCollectionTask", 8192, nullptr, 1, nullptr);
}

void loop()
{
    delay(5000);
}
