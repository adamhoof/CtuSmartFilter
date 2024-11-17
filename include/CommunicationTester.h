#pragma once
#include <vector>
#include <string>
#include "CommunicationTestable.h"

class CommunicationTester {
public:
    CommunicationTester();

    static std::vector<CommunicationAttemptResult> testDevices(const std::vector<std::reference_wrapper<CommunicationTestable>>& communicationTestableDevices);
};
