#include "tasks/KeepConnectionsAliveTask.h"
#include "MqttClientWrapper.h"

void keepConnectionsAlive(void* param)
{
    auto* params = static_cast<KeepConnectionsAliveTaskParams*>(param);
    const TickType_t wifiRestartTimeoutTicks = params->wifiRestartTimeoutTicks;

    while (true) {
        params->mqttClient.loop();
        if (WiFi.isConnected()) {
               /*loop() does nothing if _state is disconnected, but is crucial in any other phase (see switch inside the loop())
                 it needs to be called before the WiFi.connected() check to correctly detect MQTT client disconnection in the WiFi is disconnected*/
                // connect() does nothing in every _state other than disconnected, safe to call multiple times in a row even if currently connecting
                if (!mqttClient.connected()) {
                    params->mqttClient.connect();
                }
        } else { /* prerequisite to this block is WiFi.autoReconnect(true)
                    if the prerequisite ist not met for some reason, uncomment the reconnect() and 2000 ->
                    this is a way, with some compromises, to combat complete WiFi AP disappearance
                    */
            const TickType_t startTime = xTaskGetTickCount();

            while (!WiFi.isConnected() && (xTaskGetTickCount() - startTime < wifiRestartTimeoutTicks)){
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
