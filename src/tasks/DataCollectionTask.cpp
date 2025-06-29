#include "tasks/DataCollectionTask.h"
#include <ArduinoJson.h>
#include "MqttClientWrapper.h"
#include "secrets.h"

void dataCollectionTask(void* parameter)
{
    const auto* params = static_cast<MeasurementsPerformingTaskParams*>(parameter);

    static StaticJsonDocument<1024> jsonDoc;
    static char jsonOutputBuffer[1024];

    while (true) {
        jsonDoc.clear();

        for (const auto& sensor : params->sensorsToCollectMeasurementsFrom) {
            // Perform measurement and store it temporarily on the stack
            const Measurement m = sensor->performMeasurement();

            // Update the central data bank as before
            if (m.name == nullptr) {
                continue;
            }

            params->sensorDataBank.updateMeasurement(sensor->getName(), m);

            JsonObject measurementObject = jsonDoc[m.name].to<JsonObject>();

            if (strcmp(m.statusMessage.data(), "OK") == 0) {
                measurementObject["value"] = m.value;
            }
            measurementObject["status"] = m.statusMessage.data();
            vTaskDelay(pdMS_TO_TICKS(150));
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