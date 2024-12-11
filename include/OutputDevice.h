#pragma once
#include <Device.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string>
#include <vector>
#include "Measurement.h"

class OutputDevice : public Device
{
public:
    explicit OutputDevice(std::string deviceName);

    ~OutputDevice() override;

    virtual std::vector<Measurement> performMeasurements() = 0;

    std::string getName() const;

protected:
    std::string name;
    SemaphoreHandle_t dataMutex;
};
