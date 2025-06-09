#include "tasks/DataCollectionTask.h"
#include <functional>
#include <ArduinoJson.h>
#include "CollectedData.h"

void measurementsPerformingTask(void* parameter)
{
    const auto* params = static_cast<MeasurementsPerformingTaskParams*>(parameter);

    while (true) {
        for (const auto& sensor: params->sensorsToCollectMeasurementsFrom) {
            auto m = sensor->performMeasurement();
            Serial.print("Measurement name ");
            Serial.print(m.name.c_str());
            Serial.print(", ");
            Serial.print(m.unit.c_str());
            Serial.print(", ");
            Serial.print(m.value);
            Serial.print("\n");
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
