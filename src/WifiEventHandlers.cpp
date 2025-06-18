#include "WiFiEventHandlers.h"

volatile int wifiDisconnectCount = 0;
const int MAX_WIFI_DISCONNECTS = 10;

void wifiConnectedEventHandler(const arduino_event_id_t event_id)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void wifiDisconnectedEventHandler(const arduino_event_id_t event_id)
{
    Serial.printf("WiFi disconnected for the %d time\n", ++wifiDisconnectCount);
}
