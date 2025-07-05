#pragma once

#include <Preferences.h>
#include "Credentials.h"
#include <freertos/semphr.h>

enum class CredentialsStatus : uint8_t {
    OK = 0,
    UPDATE_REQUESTED = 1,
    UPDATE_SUCCESSFUL = 2,
    UPDATE_ROLLBACK = 3
};

enum class CredentialStore {
    ACTUAL,
    STAGING
};

class FlashStore {
public:
    FlashStore(const FlashStore&) = delete;
    FlashStore(FlashStore&&) = delete;
    FlashStore& operator=(const FlashStore&) = delete;
    FlashStore& operator=(FlashStore&&) = delete;

    static FlashStore& getInstance();

    CredentialsStatus getCredentialsUpdateStatus();
    bool getCredentials(Credentials& creds, CredentialStore source);

    bool stageNewCredentials(const Credentials& newCreds);
    void commitSuccessfulUpdate();
    void rollbackPendingUpdate();
    static constexpr const char* CREDENTIALS_NAMESPACE = "creds";
    static constexpr const char* STAGING_CREDENTIALS_NAMESPACE = "creds_staging";

private:
    FlashStore();
    ~FlashStore();
    Preferences prefs;
    SemaphoreHandle_t mutex;

    bool writeCredentials(Preferences& prefs, const Credentials& creds);
    bool readCredentials(Preferences& prefs, Credentials& creds);
    void setCredentialsUpdateStatus(CredentialsStatus status);
};
