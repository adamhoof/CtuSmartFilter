#include "CredentialsValidator.h"
#include <WiFi.h>

ValidationResult CredentialsValidator::validate(const Credentials& creds) {
    ValidationResult validationResult = testWifiConnection(creds.wifiSsid, creds.wifiPass);
    if (validationResult != ValidationResult::SUCCESS) {
        return validationResult;
    }

    validationResult = testMqttCertificate(creds.mqttBroker, creds.mqttPort, creds.rootCa);
    if (validationResult != ValidationResult::SUCCESS) {
        return validationResult;
    }

    return testMqttConnection(creds);
}

ValidationResult CredentialsValidator::testWifiConnection(const String& ssid, const String& pass) {
    Serial.print("VALIDATION STEP 1: Testing WiFi connection... ");
    WiFi.begin(ssid.c_str(), pass.c_str());

    int retries = 20;
    while (!WiFi.isConnected() && retries > 0) {
        delay(500);
        Serial.print(".");
        retries--;
    }

    if (!WiFi.isConnected()) {
        Serial.println("\n[FAIL] Could not connect to WiFi.");
        return ValidationResult::WIFI_FAILED;
    }

    Serial.println("\n[OK] WiFi Connected.");
    return ValidationResult::SUCCESS;
}

ValidationResult CredentialsValidator::testMqttCertificate(const String& broker, uint32_t port, const String& rootCa) {
    Serial.print("VALIDATION STEP 2: Testing certificate via TLS handshake... ");
    WiFiClientSecure testClient;
    testClient.setCACert(rootCa.c_str());

    if (!testClient.connect(broker.c_str(), port)) {
        Serial.println("[FAIL] Could not establish TLS connection. Broker unreachable or bad certificate.");
        return ValidationResult::MQTT_CERT_FAILED;
    }

    Serial.println("[OK] TLS Handshake successful.");
    testClient.stop();
    return ValidationResult::SUCCESS;
}

ValidationResult CredentialsValidator::testMqttConnection(const Credentials& creds) {
    Serial.print("VALIDATION STEP 3: Testing full MQTT authentication... ");
    espMqttClientSecure mqttC(espMqttClientTypes::UseInternalTask::NO);

    mqttC.setCACert(creds.rootCa.c_str());
    mqttC.setCredentials(creds.mqttUser.c_str(), creds.mqttPass.c_str());
    mqttC.setServer(creds.mqttBroker.c_str(), creds.mqttPort);
    mqttC.setClientId("TEST_CLIENT");
    mqttC.setCleanSession(true);

    mqttC.connect();

    int retries = 50; 
    while (!mqttC.connected() && retries > 0) {
        mqttC.loop();
        delay(300);
        Serial.print(".");
        retries--;
    }

    if (!mqttC.connected()) {
        Serial.println("\n[FAIL] MQTT connection failed. Check user/pass or client ID.");
        return ValidationResult::MQTT_AUTH_FAILED;
    }

    Serial.println("\n[OK] MQTT authentication successful.");
    mqttC.disconnect();
    return ValidationResult::SUCCESS;
}
