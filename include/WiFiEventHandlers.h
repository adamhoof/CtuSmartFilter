#pragma once
#include <espMqttClient.h>

void wifiConnectedEventHandler(arduino_event_id_t event_id);

void wifiDisconnectedEventHandler(arduino_event_id_t event_id);
