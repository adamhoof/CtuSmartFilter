#pragma once
#include <espMqttClient.h>

struct KeepConnectionsAliveTaskParams
{
  espMqttClientSecure& mqttClient;
  TickType_t wifiRestartTimeoutTicks = pdMS_TO_TICKS(10000);
};

void keepConnectionsAlive(void* params);
