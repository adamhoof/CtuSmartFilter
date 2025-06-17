#pragma once
#include <vector>
#include "CommunicationTestable.h"

class CommunicationTester {
public:
    CommunicationTester();

    static std::vector<CommunicationAttemptResult> testDevices(const std::vector<CommunicationTestable*>& communicationTestableDevices);
};
