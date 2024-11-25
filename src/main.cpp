#include <Arduino.h>
#include <CO2Sensor.h>
#include <CommunicationTester.h>
#include <DifferentialPressureSensor.h>
#include <PWMFan.h>
#include <PWMHeatingPad.h>
#include <TemperatureHumiditySensor.h>
#include <Wire.h>
#include <ThermocoupleSensor.h>
#include <ArduinoJson.h>
#include <freertos/task.h>

DifferentialPressureSensor differentialPressureSensor("FilterDifferentialPressureSensor", 0x25);
CO2Sensor co2Sensor("RoomCO2Sensor", 0x62);
TemperatureHumiditySensor temperatureHumiditySensor("RoomTemperatureHumiditySensor", 0x40);

ThermocoupleSensor thermocoupleSensor("FilterThermocoupleSensor", 23, 19, 18);

PWMFan pwmFan("PWMFan", 26,1);
PWMHeatingPad pwmHeatingPad("PWMHeatingPad", 16,2);

void initializeBusses()
{
    Wire.begin();
    Serial.begin(9600);
}

void initializeDevices(const std::vector<std::reference_wrapper<Device>>& devices)
{
    for (auto& device : devices) {
        device.get().init();
    }
}

void runConnectionTests(const std::vector<std::reference_wrapper<CommunicationTestable>>& devices)
{
    const auto results = CommunicationTester::testDevices(devices);
    for (const auto& r : results) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: ": "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}

struct DeviceMeasurements {
    std::string deviceName;
    std::vector<Measurement> measurements;
};
using CollectedData = std::vector<DeviceMeasurements>;

void collectData(const std::vector<std::reference_wrapper<OutputDevice>>& devices, CollectedData& collectedData)
{
    for (const auto& device : devices) {
        collectedData.push_back({device.get().getName(), device.get().performMeasurements()});
        delay(50);
    }
}

void serializeToJson(const CollectedData& collectedData) {
    JsonDocument doc;

    for (const auto& deviceData : collectedData) {
        for (const auto& measurement : deviceData.measurements) {
            JsonObject measurementObject = doc[measurement.name.c_str()].to<JsonObject>();
            measurementObject["value"] = measurement.value;
            measurementObject["unit"] = measurement.unit.c_str();
        }
    }

    serializeJsonPretty(doc, Serial);
}

void dataCollectionTask(void* parameter)
{
    const std::vector<std::reference_wrapper<OutputDevice>> devices = {
         differentialPressureSensor,co2Sensor,temperatureHumiditySensor, thermocoupleSensor};

    while (true) {
        CollectedData collectedData;
        collectData(devices, collectedData);
        serializeToJson(collectedData);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void setup()
{
    initializeBusses();
    initializeDevices({differentialPressureSensor,co2Sensor,temperatureHumiditySensor,thermocoupleSensor, pwmFan, pwmHeatingPad});
    runConnectionTests({differentialPressureSensor, co2Sensor, temperatureHumiditySensor, thermocoupleSensor});

    xTaskCreatePinnedToCore(
        dataCollectionTask,
        "DataCollectionTask",
        8192,
        nullptr,
        1,
        nullptr,
        1
    );
}

void loop()
{
    delay(5000);
}
