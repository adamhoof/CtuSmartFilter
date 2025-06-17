#include "tasks/DataPublishingTask.h"
#include <ArduinoJson.h>

#include "secrets.h"

void dataPublishingTask(void* parameter) {
    const auto* params = static_cast<DataPublishingTaskParams*>(parameter);

    static StaticJsonDocument<1024> jsonDoc;
    static char jsonOutputBuffer[1024];

    while (true) {
        jsonDoc.clear();

        for (const auto& sensor : params->devicesWhoseDataToPublish) {
            Measurement m = params->sensorDataBank.getMeasurement(sensor->getName());

            if (m.name == nullptr) {
                continue;
            }

            JsonObject measurementObject = jsonDoc[m.name].to<JsonObject>();

            if (strcmp(m.statusMessage.data(), "OK") == 0) {
                measurementObject["value"] = m.value;
            }
            measurementObject["status"] = m.statusMessage.data();

            Serial.printf("Name: %s, Value: %f, Unit: %s, Status: %s\n", m.name, m.value, m.unit, m.statusMessage.data());
        }

        if (jsonDoc.overflowed()) {
            Serial.println("FATAL: JSON document has overflown increase StaticJsonDocument size.");
        }

        serializeJson(jsonDoc, jsonOutputBuffer, sizeof(jsonOutputBuffer));

        params->mqttClient.publish(DATA_TOPIC, 1, false, jsonOutputBuffer);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
