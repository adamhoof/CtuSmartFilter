#include "tasks/DataCollectionTask.h"
#include <functional>
#include <ArduinoJson.h>
#include "CollectedData.h"

void measurementsPerformingTask(void* parameter) {
    const auto* params = static_cast<MeasurementsPerformingTaskParams*>(parameter);

    while (true) {
        for (const auto& device : params->devicesToCollectMeasurementsFrom) {
            device.get().performMeasurements();
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
