#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "CommunicationTestable.h"
#include "Device.h"
#include "Measurement.h"

class SensorDevice : public Device
{
public:
    explicit SensorDevice(const char* deviceName, const char* measurementName,
                          const char* measurementUnit, uint64_t measurementRefreshMS, SemaphoreHandle_t commsMutex);

    ~SensorDevice() override;

    Measurement performMeasurement();
    virtual CommunicationAttemptResult testCommunication() = 0;
    bool isReliable() const;
    void updateMeasurementRefreshMS(uint64_t measurementRefreshMS);
    uint64_t getMeasurementRefreshMS() const;

protected:
    virtual Measurement doMeasurement() = 0;

    Measurement newValidMeasurement(double value) const;
    Measurement newInvalidMeasurement(const char* errorMessage) const;

    const char* measurementName;
    const char* measurementUnit;
    const SemaphoreHandle_t commsMutex;
    SemaphoreHandle_t valueMutex;

    uint32_t consecutiveMeasurementFailures = 0;
    Measurement lastMeasurement;
    TickType_t lastMeasurementTime = 0;
    TickType_t measurementRefresh;

    enum class ReliabilityStatus {
        RELIABLE,
        UNRELIABLE,
        NOT_REGISTERED
    };
    ReliabilityStatus reliabilityStatus = ReliabilityStatus::RELIABLE;
    const uint8_t MAX_CONSECUTIVE_FAILURES = 3;
};
