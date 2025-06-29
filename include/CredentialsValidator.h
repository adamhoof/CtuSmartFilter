#pragma once

#include "FlashStore/Credentials.h"
#include "MqttClientWrapper.h"

enum class ValidationResult {
    SUCCESS,
    WIFI_FAILED,
    MQTT_CERT_FAILED,
    MQTT_AUTH_FAILED
};

class CredentialsValidator {
public:
    ValidationResult validate(const Credentials& creds);

private:
    ValidationResult testWifiConnection(const String& ssid, const String& pass);
    ValidationResult testMqttCertificate(const String& broker, uint32_t port, const String& rootCa);
    ValidationResult testMqttConnection(const Credentials& creds);
};