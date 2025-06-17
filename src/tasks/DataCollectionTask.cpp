#include "tasks/DataCollectionTask.h"
#include <functional>
#include <ArduinoJson.h>
#include "CollectedData.h"

void dataCollectionTask(void* parameter)
{
    const auto* params = static_cast<MeasurementsPerformingTaskParams*>(parameter);
    while (true) {
        for (const auto& sensor: params->sensorsToCollectMeasurementsFrom) {
            params->sensorDataBank.updateMeasurement(sensor->getName(), sensor->performMeasurement());
            vTaskDelay(pdMS_TO_TICKS(150));
        }
        vTaskDelay(pdMS_TO_TICKS(7000));
    }
}
