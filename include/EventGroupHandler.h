#pragma once

#include <freertos/event_groups.h>

extern EventGroupHandle_t connectionEventGroup;
extern const EventBits_t MQTT_RECONNECT_REQUEST_BIT;