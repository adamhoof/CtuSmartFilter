#pragma once

#include <Arduino.h>

struct Credentials
{
    String wifiSsid;
    String wifiPass;
    String mqttUser;
    String mqttPass;
    String mqttBroker;
    String rootCa;
    uint32_t mqttPort;
};
