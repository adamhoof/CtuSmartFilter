#pragma once
#include <CommunicationTestable.h>
#include <SensorDevice.h>
#include "I2CDevice.h"
#include <HTU21D.h>
#include "Measurement.h"

class HumiditySensor : public I2CDevice, public SensorDevice, public CommunicationTestable {
public:
    HumiditySensor(const char* name, byte address, const HTU21D& htu, uint64_t measurementRefreshMS, SemaphoreHandle_t commsMutex);

    void init() override;

    CommunicationAttemptResult testCommunication() override;

private:
    Measurement doMeasurement() override;
    HTU21D htu21d;
};
