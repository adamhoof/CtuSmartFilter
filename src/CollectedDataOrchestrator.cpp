#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <vector>
#include <string>
#include "CollectedDataOrchestrator.h"

CollectedDataOrchestrator::CollectedDataOrchestrator()
{
    dataMutex = xSemaphoreCreateMutex();
}

CollectedDataOrchestrator::~CollectedDataOrchestrator()
{
    if (dataMutex) {
        vSemaphoreDelete(dataMutex);
    }
}

void CollectedDataOrchestrator::setData(const CollectedData& data)
{
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    sharedData = data;
    xSemaphoreGive(dataMutex);
}

CollectedData CollectedDataOrchestrator::getData() const
{
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    CollectedData copy = sharedData;
    xSemaphoreGive(dataMutex);
    return copy;
}

CollectedDataOrchestrator collectedDataOrchestrator = CollectedDataOrchestrator();