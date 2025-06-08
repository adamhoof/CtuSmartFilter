#pragma once
#include <espMqttClient.h>

struct KeepConnectionsAliveTaskParams
{
  espMqttClientSecure& mqttClient;
};

void keepConnectionsAlive(void* params);
