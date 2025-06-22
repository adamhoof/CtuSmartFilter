#pragma once
#include <freertos/semphr.h>

class LockGuard {
public:
    explicit LockGuard(SemaphoreHandle_t mutex);
    ~LockGuard();

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard(LockGuard&&) = delete;
    LockGuard& operator=(LockGuard&&) = delete;

private:
    SemaphoreHandle_t _mutex;
};