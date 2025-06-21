#include "MqttClientWrapper.h"
#include "secrets.h"
#include "tasks/KeepConnectionsAliveTask.h"

espMqttClientSecure mqttClient(espMqttClientTypes::UseInternalTask::NO);

void configureMqttClient()
{
    mqttClient.setCACert(ROOT_CA_CHAIN);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setClientId(MQTT_CLIENT_ID);
    mqttClient.setCleanSession(true);
    mqttClient.setKeepAlive(15);
}

void connectMqttClientBlocking()
{
    Serial.println("Connecting to MQTT broker...");
    mqttClient.connect();
    while (!mqttClient.connected()) {
        Serial.print(".");
        delay(300);
    }
}

void onMqttConnect(const bool sessionPresent)
{
    Serial.printf("Connected to MQTT broker with client ID %s\n", mqttClient.getClientId());
    // TODO SUBSCRIBE TO CONTROL TOPICS
}

void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason)
{
    Serial.printf("Disconnected from MQTT: %u.\n", static_cast<uint8_t>(reason));
}

void onMqttSubscribe(const uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, const size_t len)
{
    Serial.println("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    for (size_t i = 0; i < len; ++i) {
        Serial.print("  qos: ");
        Serial.println(static_cast<uint8_t>(codes[i]));
    }
}

void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload,
                   const size_t len, const size_t index, const size_t total)
{
    (void)payload;
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(properties.qos);
    Serial.print("  dup: ");
    Serial.println(properties.dup);
    Serial.print("  retain: ");
    Serial.println(properties.retain);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
}

void onMqttPublish(const uint16_t packetId)
{
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
}
