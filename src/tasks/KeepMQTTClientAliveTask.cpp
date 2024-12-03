#include "tasks/KeepMQTTClientAliveTask.h"
#include "MqttClient.h"

void keepMqttClientAlive(void* param)
{
    auto* params = static_cast<KeepMQTTClientAliveTaskParams*>(param);

    while (true) {
        if (reconnectMqtt) {
            connectMqttClient();
        }
        else {
            params->mqttClient.loop();
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
