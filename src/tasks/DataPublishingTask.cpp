#include "tasks/DataPublishingTask.h"

#include "CollectedData.h"
#include <ArduinoJson.h>
#include <OutputDevice.h>
#include <secrets.h>

namespace
{
    void serializeToJson(const CollectedData& collectedData, char* buffer, const size_t bufferSize)
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
}

void dataPublishingTask(void* parameter)
{
    const auto* params = static_cast<DataPublishingTaskParams*>(parameter);

    while (true) {
        CollectedData collectedData;

        for (const auto& device: params->devicesWhoseDataToPublish) {
            collectedData.push_back({device->getName(), device->performMeasurements()});
        }

        constexpr size_t bufferSize = 1024;
        char jsonBuffer[bufferSize];
        serializeToJson(collectedData, jsonBuffer, bufferSize);

        params->mqttClient.publish(DATA_TOPIC, 1, false, jsonBuffer);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
