#pragma once
#include <espMqttClient.h>
#include <freertos/task.h>

extern espMqttClientSecure mqttClient;


void configureMqttClient();
void connectMqttClientBlocking();
void onMqttConnect(bool sessionPresent);
void keepConnectionsAlive(void* params);
void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);