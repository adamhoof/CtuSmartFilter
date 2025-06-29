#pragma once
#include <espMqttClient.h>

struct KeepConnectionsAliveTaskParams
{
  espMqttClientSecure& mqttClient;
  TickType_t wifiRestartTimeoutTicks;
};

void networkTask(void* params);
