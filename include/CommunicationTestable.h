#pragma once
#include <string>

enum status { SUCCESS, FAILURE };

struct CommunicationAttemptResult {
    status resultStatus;
    std::string message;
};

class CommunicationTestable {
public:
    virtual ~CommunicationTestable() = default;
    virtual CommunicationAttemptResult testCommunication() = 0;
};
