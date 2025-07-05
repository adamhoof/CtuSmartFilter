#include "MqttClientWrapper.h"
#include "Credentials.h"
#include "tasks/NetworkTask.h"

espMqttClientSecure mqttClient(espMqttClientTypes::UseInternalTask::NO);
QueueHandle_t mqttPublishQueue;

const char* MQTT_DATA_TOPIC ="demo/hofmaad2/status";
const char* MQTT_CREDENTIALS_STATUS_TOPIC ="demo/hofmaad2/credentials_status";
const char* MQTT_CLIENT_ID = "ESP32_CTU_SMART_FILTER";

void configureMqttClient(const Credentials& credentials)
{
    mqttPublishQueue = xQueueCreate(10, sizeof(MqttPublishMessage));
    mqttClient.setCACert(credentials.rootCa.c_str());
    mqttClient.setCredentials(credentials.mqttUser.c_str(), credentials.mqttPass.c_str());
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(credentials.mqttBroker.c_str(), credentials.mqttPort);
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

void queueMqttMessage(const char* topic, const char* payload, const uint8_t qos, const bool retain)
{
    MqttPublishMessage msg{};

    snprintf(msg.topic, sizeof(msg.topic), "%s", topic);
    snprintf(msg.payload, sizeof(msg.payload), "%s", payload);

    msg.qos = qos;
    msg.retain = retain;

    if (xQueueSend(mqttPublishQueue, &msg, 0) != pdPASS) {
        Serial.println("Failed to queue MQTT message. Queue full.");
    }
}
