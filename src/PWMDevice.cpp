#include "PWMDevice.h"
#include <algorithm>
#include <Arduino.h>
#include <esp32-hal.h>

PWMDevice::PWMDevice(const uint8_t pwmPin)
    : pwmPin(pwmPin), powerLevel(0)
{
}

void PWMDevice::increasePower(const uint8_t value) {
    powerLevel = std::min(100, powerLevel + value);
    setPower(powerLevel);
}

void PWMDevice::decreasePower(const uint8_t value) {
    powerLevel = std::max(0, powerLevel - value);
    setPower(powerLevel);
}

void PWMDevice::setPower(const uint8_t value) {
    if (powerLevel == value) {
        return;
    }
    powerLevel = map(0, 100, 0, 255, value);
    analogWrite(pwmPin, powerLevel);
}

uint8_t PWMDevice::getPower() const {
    return powerLevel;
}
