#include "CommunicationTester.h"

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
    }

    return results;
}
