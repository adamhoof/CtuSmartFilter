#pragma once
#include <vector>
#include "MqttClientWrapper.h"
#include "SensorDataBank.h"
#include "SensorDevice.h"

struct DataPublishingTaskParams
{
    espMqttClientSecure& mqttClient;
    std::vector<SensorDevice*> devicesWhoseDataToPublish;
    SensorDataBank& sensorDataBank;
};

void dataPublishingTask(void* parameter);
