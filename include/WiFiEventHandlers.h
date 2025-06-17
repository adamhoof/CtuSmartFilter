#pragma once
#include <espMqttClient.h>

extern volatile int wifiDisconnectCount;
extern const int MAX_WIFI_DISCONNECTS;

void wifiConnectedEventHandler(arduino_event_id_t event_id);

void wifiDisconnectedEventHandler(arduino_event_id_t event_id);
