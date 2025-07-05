#pragma once
#include <espMqttClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "Credentials.h"

extern espMqttClientSecure mqttClient;
extern QueueHandle_t mqttPublishQueue;

extern const char* MQTT_DATA_TOPIC;
extern const char* MQTT_CREDENTIALS_STATUS_TOPIC;
extern const char* MQTT_CLIENT_ID;

struct MqttPublishMessage
{
    char topic[128];
    char payload[1024];
    uint8_t qos;
    bool retain;
};

void configureMqttClient(const Credentials& credentials);
void connectMqttClientBlocking();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);
void queueMqttMessage(const char* topic, const char* payload, uint8_t qos = 1, bool retain = false);