#include "EventGroupHandler.h"

EventGroupHandle_t connectionEventGroup = xEventGroupCreate();
const EventBits_t MQTT_RECONNECT_REQUEST_BIT = (1 << 0);