#pragma once
#include <string>
#include <vector>
#include "Measurement.h"

struct DeviceMeasurements {
    std::string deviceName;
    std::vector<Measurement> measurements;
};