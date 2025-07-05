#pragma once
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "CommunicationTestable.h"

class CommunicationTester {
public:
    CommunicationTester();

    static std::vector<CommunicationAttemptResult> testDevices(const std::vector<CommunicationTestable*>& communicationTestableDevices, SemaphoreHandle_t commsMutex);
};
