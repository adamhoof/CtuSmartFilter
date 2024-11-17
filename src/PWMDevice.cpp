#include "PWMDevice.h"
#include <algorithm>
#include <esp32-hal-ledc.h>

PWMDevice::PWMDevice(const uint8_t pwmPin, const uint8_t pwmChannel)
    : pwmPin(pwmPin), pwmChannel(pwmChannel), powerLevel(0)
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
    ledcWrite(pwmChannel, powerLevel);
}

uint8_t PWMDevice::getPower() const {
    return powerLevel;
}
