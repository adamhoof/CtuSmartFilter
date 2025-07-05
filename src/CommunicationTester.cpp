#include "CommunicationTester.h"
#include <esp32-hal.h>
#include "LockGuard.h"

CommunicationTester::CommunicationTester() = default;

std::vector<CommunicationAttemptResult> CommunicationTester::testDevices(const std::vector<CommunicationTestable*>& communicationTestableDevices, SemaphoreHandle_t commsMutex)
{
    std::vector<CommunicationAttemptResult> results;

    for (const auto device : communicationTestableDevices) {
        LockGuard lockGuard(commsMutex);
        const auto result = device->testCommunication();
        results.push_back({
            result.resultStatus == SUCCESS ? SUCCESS : FAILURE,
            result.message
        });
        delay(100);
    }

    return results;
}
