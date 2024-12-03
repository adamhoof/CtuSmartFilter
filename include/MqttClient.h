#pragma once
#include <espMqttClient.h>

extern espMqttClientSecure mqttClient;
extern bool reconnectMqtt;
extern uint32_t lastReconnect;

void connectMqttClient();
void configureMqttClient();

void onMqttConnect(bool sessionPresent);
void keepMqttClientAlive(void* params);
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);