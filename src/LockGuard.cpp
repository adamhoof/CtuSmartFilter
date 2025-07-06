#include "LockGuard.h"

LockGuard::LockGuard(SemaphoreHandle_t mutex) : mutex(mutex) {
    if (mutex != nullptr) {
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
}

LockGuard::~LockGuard() {
    if (mutex != nullptr) {
        xSemaphoreGive(mutex);
    }
}