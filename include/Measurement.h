#pragma once

#define MAX_STATUS_MSG_LEN 128
#include <array>

struct Measurement {
    const char* name{""};
    double value{};
    const char* unit{""};
    std::array<char, MAX_STATUS_MSG_LEN> statusMessage{};
};
