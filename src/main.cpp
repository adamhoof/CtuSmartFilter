#include <Arduino.h>
#include <SPI.h>
#include "CO2Sensor.h"
#include "CommunicationTester.h"
#include "DifferentialPressureSensor.h"
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "TemperatureSensor.h"
#include <Wire.h>
#include "ThermocoupleSensor.h"
#include <WiFi.h>
#include <freertos/task.h>
#include <tasks/DataCollectionTask.h>
#include <tasks/FilterCycleTask.h>
#include <tasks/NetworkTask.h>
#include "HumiditySensor.h"
#include "MqttClientWrapper.h"
#include "SensorDataBank.h"
#include <Preferences.h>
#include "CredentialsValidator.h"
#include "FlashStore/FlashStore.h"

static constexpr uint8_t csPin = 23, sckPin = 18, misoPin = 19;

void initializeBusses()
{
    Wire.begin();
    Serial.begin(9600);
    // MAX6675 is output only, no mosi needed
    SPI.begin(sckPin, misoPin);
}

void initializeDevices(const std::vector<Device*>& devices)
{
    for (auto& device: devices) {
        device->init();
        delay(100);
    }
}

void runConnectionTests(const std::vector<CommunicationTestable*>& devices)
{
    for (const auto& r: CommunicationTester::testDevices(devices)) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}

void fetchCredentials(Credentials& credentials, FlashStore& flashStore, const CredentialsStatus& credentialsStatus)
{
    if (credentialsStatus == CredentialsStatus::UPDATE_REQUESTED) {
        if (!flashStore.getCredentials(credentials, CredentialStore::STAGING)) {
            flashStore.rollbackPendingUpdate();
            ESP.restart();
        }
        CredentialsValidator credentialsValidator;
        const ValidationResult validationResult = credentialsValidator.validate(credentials);
        if (validationResult == ValidationResult::SUCCESS) {
            flashStore.commitSuccessfulUpdate();
        } else {
            flashStore.rollbackPendingUpdate();
        }
        Serial.println("Restarting");
        ESP.restart();
    } else {
        flashStore.getCredentials(credentials, CredentialStore::ACTUAL);
    }
}

void publishCredentialsStatus(const CredentialsStatus& credentialsStatus)
{
    const char* statusMessage;
    switch (credentialsStatus) {
        case CredentialsStatus::OK:
            statusMessage = "OK";
            break;
        case CredentialsStatus::UPDATE_SUCCESSFUL:
            statusMessage = "Update successful";
            break;
        case CredentialsStatus::UPDATE_ROLLBACK:
            statusMessage = "Update rollback";
            break;
        default:
            statusMessage = "Unknown credentials status";
            break;
    }
    Serial.printf("Publishing credentials status: %s\n",statusMessage);
    queueMqttMessage(MQTT_CREDENTIALS_STATUS_TOPIC,statusMessage);
}

void setup()
{
    initializeBusses();
    Preferences preferences;
    preferences.begin("creds", false);
    preferences.putUChar("update_status", static_cast<uint8_t>(CredentialsStatus::OK));
    preferences.end();

    static FlashStore& flashStore = FlashStore::getInstance();
    static Credentials credentials;
    const CredentialsStatus credentialsStatus = flashStore.getCredentialsUpdateStatus();
    // TODO if failed to fetch from actual, try to fetch from staging
    fetchCredentials(credentials, flashStore, credentialsStatus);

    configureMqttClient(credentials);
    publishCredentialsStatus(credentialsStatus);

    static DifferentialPressureSensor differentialPressureSensor("FilterDifferentialPressureSensor", 0x25);
    static CO2Sensor co2Sensor("RoomCO2Sensor", 0x62);

    // To keep things simple, the multi value HTU21D sensor is split into 2 different sensors, that both use the same physical sensor
    static HTU21D htu21d;
    static TemperatureSensor temperatureSensor("RoomTemperatureSensor", 0x40, htu21d);
    static HumiditySensor humiditySensor("RoomHumiditySensor", 0x40, htu21d);

    static ThermocoupleSensor thermocoupleSensor("FilterThermocoupleSensor", csPin);

    static PWMFan pwmFan("PWMFan", 26);
    static PWMHeatingPad pwmHeatingPad("PWMHeatingPad", 16);

    static SensorDataBank sensorDataBank(std::vector<const char*>{
        temperatureSensor.getName(),
        humiditySensor.getName(),
        differentialPressureSensor.getName(),
        co2Sensor.getName(),
        thermocoupleSensor.getName(),
    });

    // avoid dangling mosi pin state when disconnected
    pinMode(misoPin, INPUT_PULLUP);
    initializeDevices({
        &thermocoupleSensor,
        &differentialPressureSensor,
        &co2Sensor,
        &temperatureSensor,
        &humiditySensor, /*,
        &pwmFan,
        &pwmHeatingPad*/
    });

    runConnectionTests({
        &differentialPressureSensor,
        &co2Sensor,
        &temperatureSensor,
        &humiditySensor,
        &thermocoupleSensor
    });

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent([](const arduino_event_id_t event_id) {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    // TODO same mechanism as in the keep connections alive - no infinite boi, mby extract to function
    WiFi.begin(credentials.wifiSsid, credentials.wifiPass);
    while (!WiFi.isConnected()) {
        delay(300);
        Serial.print(".");
    }

    static auto dataCollectionTaskParams = DataCollectionTaskParams{
        .sensorsToCollectMeasurementsFrom = {
            &differentialPressureSensor,
            &co2Sensor,
            &temperatureSensor,
            &humiditySensor,
            &thermocoupleSensor
        },
        .sensorDataBank = sensorDataBank
    };
    xTaskCreate(dataCollectionTask, "dataCollectionTask", 8192, &dataCollectionTaskParams, 1, nullptr);

    auto filterRegenTaskParams = new FilterRegenTaskParams{
            .fan = pwmFan,
            .heatingPad = pwmHeatingPad,
            .sensorDataBank = sensorDataBank,
            .conf = FilterRegenTaskConfig{}
        };
    xTaskCreate(filterCycleTask, "filterRegenTask", 8192, filterRegenTaskParams, 2, nullptr);

    static auto keepConnectionsAliveTaskParams = KeepConnectionsAliveTaskParams{
        .mqttClient = mqttClient,
        .wifiRestartTimeoutTicks = pdMS_TO_TICKS(15000)
    };
    xTaskCreate(networkTask, "networkTask", 10000, &keepConnectionsAliveTaskParams, 1, nullptr);

    // after setup is done with setting things up, delete the setup task and thus the infinite loop, freeing its stack and removing it from the scheduler
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelete(nullptr);
}

void loop()
{}
