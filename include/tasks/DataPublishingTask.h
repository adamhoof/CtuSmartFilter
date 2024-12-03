#pragma once
#include <vector>

#include "MqttClient.h"

class OutputDevice;

struct DataPublishingTaskParams
{
    espMqttClientSecure& mqttClient;
    std::vector<std::reference_wrapper<OutputDevice>> devicesWhoseDataToPublish;
};

void dataPublishingTask(void* parameter);
