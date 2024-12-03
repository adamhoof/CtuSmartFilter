#pragma once
#include <espMqttClient.h>

struct KeepMQTTClientAliveTaskParams
{
  espMqttClientSecure& mqttClient;
  EventGroupHandle_t& connectionEventGroup;
  const EventBits_t& mqttConnectionRequestBit;
};

void keepMqttClientAlive(void* params);
