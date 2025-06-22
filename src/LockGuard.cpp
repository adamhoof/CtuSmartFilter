#include "LockGuard.h"

LockGuard::LockGuard(SemaphoreHandle_t mutex) : _mutex(mutex) {
    if (_mutex != nullptr) {
        xSemaphoreTake(_mutex, portMAX_DELAY);
    }
}

LockGuard::~LockGuard() {
    if (_mutex != nullptr) {
        xSemaphoreGive(_mutex);
    }
}