#pragma once

#include <array>
#include <string>
#include <vector>
#include <unordered_map>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "Measurement.h"

enum class ReliabilityStatus {
    RELIABLE,
    UNRELIABLE,
    NOT_REGISTERED
};

class SensorDataBank {
public:
    explicit SensorDataBank(const std::vector<const char*>& sensorDeviceNames);
    ~SensorDataBank();
    void updateMeasurement(const char* sensorDeviceName, const Measurement& measurement);
    Measurement getMeasurement(const char* sensorDeviceName);

    ReliabilityStatus getReliabilityStatus(const char* sensorDeviceName);

private:
    struct SensorData {
        Measurement latestMeasurement;
        uint32_t totalFailureCount = 0;
        ReliabilityStatus reliability = ReliabilityStatus::RELIABLE;
    };

    std::unordered_map<std::string, SensorData> sensorDataEntriesMap;
    SemaphoreHandle_t mutex;
    const uint32_t MAX_TOTAL_FAILURES = 5;
};