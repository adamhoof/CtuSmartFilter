#pragma once
#include <espMqttClient.h>

struct KeepMQTTClientAliveTaskParams
{
  espMqttClientSecure& mqttClient;
};

void keepMqttClientAlive(void* params);
