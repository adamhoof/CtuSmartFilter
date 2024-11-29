#include <espMqttClient.h>

espMqttClientSecure mqttClient(espMqttClientTypes::UseInternalTask::NO);
bool reconnectMqtt = false;

void connectMqttClient()
{
    Serial.println("Connecting to MQTT...");
    if (!mqttClient.connect()) {
        reconnectMqtt = true;
        Serial.println("Connecting failed.");
        return;
    }
    reconnectMqtt = false;
}

void keepMqttClientAlive(void* params)
{
    while (true) {
        if (reconnectMqtt) {
            connectMqttClient();
        }
        else {
            mqttClient.loop();
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void onMqttConnect(const bool sessionPresent)
{
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
}

void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason)
{
    Serial.printf("Disconnected from MQTT: %u.\n", static_cast<uint8_t>(reason));
    reconnectMqtt = true;
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
