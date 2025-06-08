#pragma once
#include <vector>

#include "MqttClientWrapper.h"

class OutputDevice;

struct DataPublishingTaskParams
{
    espMqttClientSecure& mqttClient;
    std::vector<OutputDevice*> devicesWhoseDataToPublish;
};

void dataPublishingTask(void* parameter);
