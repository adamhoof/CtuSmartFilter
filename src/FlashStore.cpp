#include "FlashStore.h"
#include <Arduino.h>
#include "LockGuard.h"

FlashStore::FlashStore() {
    mutex = xSemaphoreCreateMutex();
}

FlashStore::~FlashStore() {
    vSemaphoreDelete(mutex);
}

FlashStore& FlashStore::getInstance() {
    static FlashStore instance;
    return instance;
}

CredentialsStatus FlashStore::getCredentialsUpdateStatus() {
    LockGuard lock(mutex);
    prefs.begin(CREDENTIALS_NAMESPACE, true);
    auto statusValue = prefs.getUChar("update_status");
    prefs.end();
    return static_cast<CredentialsStatus>(statusValue);
}

bool FlashStore::getCredentials(Credentials& creds, const CredentialStore source) {
    LockGuard lock(mutex);
    const char* namespaceToLoad = (source == CredentialStore::STAGING)
        ? STAGING_CREDENTIALS_NAMESPACE
        : CREDENTIALS_NAMESPACE;

    if (!prefs.begin(namespaceToLoad, true)) {
        return false;
    }

    const bool success = readCredentials(prefs, creds);
    prefs.end();
    return success;
}

bool FlashStore::stageNewCredentials(const Credentials& newCreds) {
    LockGuard lock(mutex);
    Serial.println("FlashStore: Staging new credentials...");

    if (!prefs.begin(STAGING_CREDENTIALS_NAMESPACE, false) ||
        !writeCredentials(prefs, newCreds)) {
        Serial.printf("FlashStore: ERROR - could not write to %s namespace\n", STAGING_CREDENTIALS_NAMESPACE);
        return false;
    }
    prefs.end();

    setCredentialsUpdateStatus(CredentialsStatus::UPDATE_REQUESTED);

    Serial.println("FlashStore: Staging successful. Ready for reboot.");
    return true;
}

void FlashStore::commitSuccessfulUpdate() {
    LockGuard lock(mutex);
    Serial.println("FlashStore: Committing successful update");
    Credentials stagedCreds;

    if (!prefs.begin(STAGING_CREDENTIALS_NAMESPACE, true) ||
        !readCredentials(prefs, stagedCreds)) {
        setCredentialsUpdateStatus(CredentialsStatus::UPDATE_ROLLBACK);
        return;
    };
    prefs.end();

    if (!prefs.begin(CREDENTIALS_NAMESPACE, false) ||
        !writeCredentials(prefs, stagedCreds)) {
        Serial.printf("FlashStore: ERROR - could not write to %s namespace\n", CREDENTIALS_NAMESPACE);
        setCredentialsUpdateStatus(CredentialsStatus::UPDATE_ROLLBACK);
        return;
    }
    prefs.end();

    Serial.println("FlashStore: Commit successful");
    setCredentialsUpdateStatus(CredentialsStatus::UPDATE_SUCCESSFUL);
}

void FlashStore::rollbackPendingUpdate() {
    LockGuard lock(mutex);
    Serial.println("FlashStore: rollback");

    setCredentialsUpdateStatus(CredentialsStatus::UPDATE_ROLLBACK);
}

void FlashStore::setCredentialsUpdateStatus(CredentialsStatus status) {
    prefs.begin(CREDENTIALS_NAMESPACE, false);
    prefs.putUChar("status", static_cast<uint8_t>(status));
    prefs.end();
}

bool FlashStore::writeCredentials(Preferences& prefs, const Credentials& creds) {
    size_t totalBytesWritten = 0;
    totalBytesWritten += prefs.putString("wifi_ssid", creds.wifiSsid);
    totalBytesWritten += prefs.putString("wifi_pass", creds.wifiPass);
    totalBytesWritten += prefs.putString("mqtt_user", creds.mqttUser);
    totalBytesWritten += prefs.putString("mqtt_pass", creds.mqttPass);
    totalBytesWritten += prefs.putString("mqtt_broker", creds.mqttBroker);
    totalBytesWritten += prefs.putString("root_ca", creds.rootCa);
    totalBytesWritten += prefs.putUInt("mqtt_port", creds.mqttPort);

    const size_t expectedBytes = (creds.wifiSsid.length()) +
                           (creds.wifiPass.length()) +
                           (creds.mqttUser.length()) +
                           (creds.mqttPass.length()) +
                           (creds.mqttBroker.length()) +
                           (creds.rootCa.length()) +
                           sizeof(creds.mqttPort);

    return totalBytesWritten == expectedBytes;
}

bool FlashStore::readCredentials(Preferences& prefs, Credentials& creds) {
    creds.wifiSsid = prefs.getString("wifi_ssid");
    creds.wifiPass = prefs.getString("wifi_pass");
    creds.mqttUser = prefs.getString("mqtt_user");
    creds.mqttPass = prefs.getString("mqtt_pass");
    creds.mqttBroker = prefs.getString("mqtt_broker");
    creds.rootCa = prefs.getString("root_ca");
    creds.mqttPort = prefs.getUInt("mqtt_port", 0);

    if (creds.wifiSsid.isEmpty() ||
        creds.wifiPass.isEmpty() ||
        creds.mqttUser.isEmpty() ||
        creds.mqttPass.isEmpty() ||
        creds.mqttBroker.isEmpty() ||
        creds.rootCa.isEmpty() ||
        creds.mqttPort == 0) {
        return false;
    }
    return true;
}