#include "WiFiEventHandlers.h"
#include <MqttClient.h>

void wifiConnectedEventHandler(const arduino_event_id_t event_id)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    reconnectMqtt = true;
}

void wifiDisconnectedEventHandler(const arduino_event_id_t event_id)
{
    Serial.println("WiFi lost connection");
}
