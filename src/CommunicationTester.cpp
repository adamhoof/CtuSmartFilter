#include "CommunicationTester.h"
#include <esp32-hal.h>

CommunicationTester::CommunicationTester() = default;

std::vector<CommunicationAttemptResult> CommunicationTester::testDevices(const std::vector<CommunicationTestable*>& communicationTestableDevices)
{
    std::vector<CommunicationAttemptResult> results;

    for (const auto device : communicationTestableDevices) {
        const auto result = device->testCommunication();
        results.push_back({
            result.resultStatus == SUCCESS ? SUCCESS : FAILURE,
            result.message
        });
        delay(100);
    }

    return results;
}
