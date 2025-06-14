#include "PWMControlDevice.h"
#include <algorithm>
#include <Arduino.h>
#include <esp32-hal.h>

PWMControlDevice::PWMControlDevice(const char* name, const uint8_t pwmPin)
    : Device(name), pwmPin(pwmPin), powerLevel(0)
{
}

void PWMControlDevice::increasePower(const uint8_t byPercent) {
    powerLevel = std::min(100, powerLevel + byPercent);
    setPower(powerLevel);
}

void PWMControlDevice::decreasePower(const uint8_t byPercent) {
    powerLevel = std::max(0, powerLevel - byPercent);
    setPower(powerLevel);
}

void PWMControlDevice::setPower(const uint8_t percent) {
    if (powerLevel == percent) {
        return;
    }
    powerLevel = map(0, 100, 0, 255, percent);
    analogWrite(pwmPin, powerLevel);
}

uint8_t PWMControlDevice::getPower() const {
    return powerLevel;
}
