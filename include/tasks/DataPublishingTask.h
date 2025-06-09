#pragma once
#include <vector>
#include "MqttClientWrapper.h"

class SensorDevice;

struct DataPublishingTaskParams
{
    espMqttClientSecure& mqttClient;
    std::vector<SensorDevice*> devicesWhoseDataToPublish;
};

void dataPublishingTask(void* parameter);
