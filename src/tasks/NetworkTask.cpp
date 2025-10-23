#include "tasks/NetworkTask.h"
#include "MqttClientWrapper.h"

[[noreturn]] void networkTask(void* params)
{
    const auto* p = static_cast<KeepConnectionsAliveTaskParams*>(params);
    const TickType_t wifiRestartTimeoutTicks = p->wifiRestartTimeoutTicks;

    while (true) {
        p->mqttClient.loop();
        if (WiFi.isConnected()) {
            /*loop() does nothing if the internal _state is disconnected, but is crucial in any other phase (see switch inside the loop())
              it needs to be called before the WiFi.connected() check to correctly detect MQTT client disconnection if the WiFi is disconnected
              connect() does nothing in every _state other than disconnected, safe to call multiple times in a row even if currently connecting */
            if (!mqttClient.connected()) {
                p->mqttClient.connect();
            }
            else {
                MqttPublishMessage msg{};
                if (xQueueReceive(mqttPublishQueue, &msg, 0) == pdPASS) {
                    p->mqttClient.publish(msg.topic, msg.qos, msg.retain, msg.payload);
                }
            }
        }
        else { // WiFi is not connected
            /* prerequisite to this block is WiFi.autoReconnect(true)
               if the prerequisite ist not met for some reason, uncomment the reconnect() and 2000 ->
               this is a way, with some compromises, to combat complete WiFi AP disappearance */

            // TODO if (WiFi.getAutoConnect()) instead of this comment uptop
            const TickType_t startTime = xTaskGetTickCount();

            while (!WiFi.isConnected() && (xTaskGetTickCount() - startTime < wifiRestartTimeoutTicks)) {
                /*WiFi.reconnect();*/
                Serial.print(".");
                vTaskDelay(pdMS_TO_TICKS(300/*2000*/));
            }

            if (!WiFi.isConnected()) {
                Serial.println("\nFailed to reconnect WiFi, restarting.");
                ESP.restart();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
