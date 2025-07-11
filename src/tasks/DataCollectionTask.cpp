#include "tasks/DataCollectionTask.h"
#include <ArduinoJson.h>
#include "MqttClientWrapper.h"

void dataCollectionTask(void* params)
{
    const auto* p = static_cast<DataCollectionTaskParams*>(params);

    static StaticJsonDocument<1024> jsonDoc;
    static char jsonOutputBuffer[1024];

    while (true) {
        jsonDoc.clear();

        for (const auto& sensor : p->sensors) {
            const Measurement m = sensor->performMeasurement();

            if (m.name == nullptr) {
                continue;
            }

            JsonObject measurementObj = jsonDoc[m.name].to<JsonObject>();

            if (strcmp(m.statusMessage.data(), "OK") == 0) {
                measurementObj["value"] = m.value;
            }
            measurementObj["status"] = m.statusMessage.data();
            vTaskDelay(pdMS_TO_TICKS(15));
        }

        if (jsonDoc.overflowed()) {
            Serial.println("FATAL: JSON document has overflown. Increase StaticJsonDocument size.");
        }

        // Serialize the completed JSON document and queue it for publishing
        serializeJson(jsonDoc, jsonOutputBuffer, sizeof(jsonOutputBuffer));
        queueMqttMessage(MQTT_DATA_TOPIC, jsonOutputBuffer);

        vTaskDelay(pdMS_TO_TICKS(7000));
    }
}
