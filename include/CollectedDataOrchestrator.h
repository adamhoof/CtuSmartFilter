#pragma once
#include <Measurement.h>
#include <string>
#include <vector>
#include <freertos/semphr.h>

struct DeviceMeasurements
{
    std::string deviceName;
    std::vector<Measurement> measurements;
};

using CollectedData = std::vector<DeviceMeasurements>;

class CollectedDataOrchestrator
{
public:
    CollectedDataOrchestrator();

    ~CollectedDataOrchestrator();

    void setData(const CollectedData& data);

    CollectedData getData() const;

private:
    CollectedData sharedData;
    SemaphoreHandle_t dataMutex;
};

extern CollectedDataOrchestrator collectedDataOrchestrator;
