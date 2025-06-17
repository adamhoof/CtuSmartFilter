#include "SensorDataBank.h"
#include <Arduino.h>
#include "Measurement.h"

SensorDataBank::SensorDataBank(const std::vector<const char*>& sensorDeviceNames) {
    mutex = xSemaphoreCreateMutex();
    sensorDataEntriesMap.reserve(sensorDeviceNames.size());

    for (const char* name : sensorDeviceNames) {
        assert(name != nullptr && "Cannot register a null sensor name!");
        sensorDataEntriesMap[name] = SensorData{};
    }
}

SensorDataBank::~SensorDataBank() {
    vSemaphoreDelete(mutex);
}

void SensorDataBank::updateMeasurement(
    const char* sensorDeviceName, const Measurement& measurement) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE) {
        return;
    }

    const auto it = sensorDataEntriesMap.find(sensorDeviceName);

    if (it == sensorDataEntriesMap.end()) {
        xSemaphoreGive(mutex);
        return;
    }

    SensorData& sd = it->second;
    sd.latestMeasurement = measurement;

    if (strcmp(measurement.statusMessage.data(), "OK") != 0) {
        ++sd.consecutiveFailureCount;
    } else {
        sd.consecutiveFailureCount = 0;
    }

    if (sd.reliability == ReliabilityStatus::RELIABLE && sd.consecutiveFailureCount >= MAX_TOTAL_FAILURES) {
        sd.reliability = ReliabilityStatus::UNRELIABLE;
        Serial.printf("ALERT: Sensor '%s' has been flagged as unreliable.\n",sensorDeviceName);
    }

    xSemaphoreGive(mutex);
}

Measurement SensorDataBank::getMeasurement(const char* sensorDeviceName) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE) {
        return {nullptr, 0.0, nullptr};
    }

    auto it = sensorDataEntriesMap.find(sensorDeviceName);

    if (it == sensorDataEntriesMap.end()) {
        xSemaphoreGive(mutex);
        return {nullptr, 0.0, nullptr};
    }

    const Measurement copy = it->second.latestMeasurement;
    xSemaphoreGive(mutex);
    return copy;
}

ReliabilityStatus SensorDataBank::getReliabilityStatus(const char* sensorDeviceName) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE) {
        return ReliabilityStatus::NOT_REGISTERED;
    }

    auto it = sensorDataEntriesMap.find(sensorDeviceName);

    if (it == sensorDataEntriesMap.end()) {
        xSemaphoreGive(mutex);
        return ReliabilityStatus::NOT_REGISTERED;
    }

    const ReliabilityStatus status = it->second.reliability;
    xSemaphoreGive(mutex);
    return status;
}
