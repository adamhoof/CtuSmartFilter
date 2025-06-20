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
#include "secrets.h"
#include <WiFi.h>
#include <freertos/task.h>
#include <tasks/DataCollectionTask.h>
#include <tasks/DataPublishingTask.h>
#include <tasks/FilterRegenTask.h>
#include <tasks/KeepConnectionsAliveTask.h>
#include "HumiditySensor.h"
#include "MqttClientWrapper.h"
#include "SensorDataBank.h"
#include <Preferences.h>

static constexpr uint8_t csPin = 23, sckPin = 18, misoPin = 19;

// A struct for our flash storage demo
struct TestData
{
    int id;
    float value;
    bool enabled;
};

/*void flashTest()
{
    Preferences preferences;

    // 1. Start the Preferences library in read/write mode.

    // 2. Prepare data and attempt to write it to flash.

    // 3. Close the preferences to ensure data is committed.

    Serial.println("\nRe-opening flash to read data...");

    // 4. Re-open preferences in read-only mode.
    preferences.begin("CtuSmartFilter", true);

    // 5. Prepare a struct to hold the read data.
    TestData dataToRead;

    // 6. Attempt to read the data back.
    size_t bytesRead = preferences.getBytes("test_data", &dataToRead, sizeof(TestData));

    if (bytesRead == sizeof(TestData)) {
        Serial.println("Success! Read data from flash:");
        Serial.printf("ID: %d, Value: %.2f, Enabled: %s\n", dataToRead.id, dataToRead.value,
                      dataToRead.enabled ? "true" : "false");
    }
    else {
        Serial.println("Error: Failed to read data from flash or key does not exist.");
    }

    // 7. Close the preferences again.
    preferences.end();
}*/


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
    }
}

void runConnectionTests(const std::vector<CommunicationTestable*>& devices)
{
    for (const auto& r: CommunicationTester::testDevices(devices)) {
        Serial.print(r.resultStatus == SUCCESS ? "SUCCESS: " : "FAILURE: ");
        Serial.println(r.message.c_str());
    }
}

void setup()
{
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
    initializeBusses();
    initializeDevices({
        &differentialPressureSensor,
        &co2Sensor,
        &temperatureSensor,
        &humiditySensor,
        &thermocoupleSensor /*,
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

    configureMqttClient();

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent([](const arduino_event_id_t event_id) {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (!WiFi.isConnected()) {
        delay(300);
        Serial.print(".");
    }

    static auto dataCollectionTaskParams = MeasurementsPerformingTaskParams{
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

    /*auto filterRegenTaskParams = new FilterRegenTaskParams{
            .co2Sensor = co2Sensor,
            .fan = pwmFan,
            .heatingPad = pwmHeatingPad,
            .thermocoupleSensor = thermocoupleSensor,
            nullptr
        };
    xTaskCreate(filterRegenTask, "filterRegenTask", 8192, filterRegenTaskParams, 2, nullptr);*/

    static auto keepConnectionsAliveTaskParams = KeepConnectionsAliveTaskParams{
        .mqttClient = mqttClient,
    };
    xTaskCreate(keepConnectionsAlive, "keepConnectionsAlive", 10000, &keepConnectionsAliveTaskParams, 1, nullptr);
    static auto dataPublishingTaskParams = DataPublishingTaskParams{
        .mqttClient = mqttClient,
        .devicesWhoseDataToPublish = {
            &differentialPressureSensor, &co2Sensor, &temperatureSensor, &humiditySensor, &thermocoupleSensor
        },
        .sensorDataBank = sensorDataBank
    };
    xTaskCreate(dataPublishingTask, "dataPublishingTask", 8192, &dataPublishingTaskParams, 1, nullptr);
}

void loop()
{
    delay(10000);
}
