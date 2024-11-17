#include "CommunicationTester.h"

#include <functional>

CommunicationTester::CommunicationTester() = default;

std::vector<CommunicationAttemptResult> CommunicationTester::testDevices(const std::vector<std::reference_wrapper<CommunicationTestable>>& communicationTestableDevices)
{
    std::vector<CommunicationAttemptResult> results;

    for (const auto& device : communicationTestableDevices) {
        const auto result = device.get().testCommunication();
        results.push_back({
            result.resultStatus == SUCCESS ? SUCCESS : FAILURE,
            result.message
        });
    }

    return results;
}
