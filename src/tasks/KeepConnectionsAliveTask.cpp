#include "tasks/KeepConnectionsAliveTask.h"
#include "MqttClientWrapper.h"
#include "WiFiEventHandlers.h"

// proper concurrent handling should be applied here
// + calling individual functions does not invoke state changes if they are not required (ie. if already connecting, another connect request is not made, just returns)
// + requesting state information is atomically guarded
// also, this is super simple and functional approach that is capable of detecting any disconnection reason
void keepConnectionsAlive(void* param)
{
    auto* params = static_cast<KeepConnectionsAliveTaskParams*>(param);

    while (true) {
        if (wifiDisconnectCount >= MAX_WIFI_DISCONNECTS) {
            ESP.restart();
        }
        if (WiFi.isConnected()) {
            // loop() does nothing if _state is disconnected, but is crucial in any other phase (see switch inside the loop())
            // it needs to be called before the WiFi.connected() check to correctly detect MQTT client disconnection in the WiFi is disconnected
            params->mqttClient.loop();
            if (WiFi.isConnected()) {
                // connect() does nothing in every _state other than disconnected, safe to call multiple times in a row even if currently connecting
                if (!mqttClient.connected()) {
                    params->mqttClient.connect();
                }
            }
        } else {
            WiFi.reconnect();
            int retries = 0;
            while (!WiFi.isConnected() && retries < 8) {
                Serial.print("Reconnecting WiFi");
                Serial.print(".");
                vTaskDelay(pdMS_TO_TICKS(1000));
                retries++;
            }

            if (!WiFi.isConnected()) {
                Serial.println("\nFailed to reconnect WiFi, restarting.");
                ESP.restart();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
