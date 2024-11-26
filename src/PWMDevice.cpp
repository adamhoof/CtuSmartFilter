#include "PWMDevice.h"
#include <algorithm>
#include <esp32-hal.h>

PWMDevice::PWMDevice(const uint8_t pwmPin)
    : pwmPin(pwmPin), powerLevel(0)
{
}

void PWMDevice::increasePower(const uint8_t value) {
    powerLevel = std::min(255, powerLevel + value);
    setPower(powerLevel);
}

void PWMDevice::decreasePower(const uint8_t value) {
    powerLevel = std::max(0, powerLevel - value);
    setPower(powerLevel);
}

void PWMDevice::setPower(const uint8_t value) {
    powerLevel = value;
    analogWrite(pwmPin, powerLevel);
}

uint8_t PWMDevice::getPower() const {
    return powerLevel;
}