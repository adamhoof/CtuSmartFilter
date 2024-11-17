#include "PWMFan.h"

#include <esp32-hal-ledc.h>

PWMFan::PWMFan(const std::string& name, const uint8_t pwmPin, const uint8_t pwmChannel)
    : InputDevice(name), PWMDevice(pwmPin, pwmChannel)
{
}

void PWMFan::init()
{
    ledcSetup(pwmChannel, 25000, 8);
    ledcAttachPin(pwmPin, pwmChannel);
    setPower(0);
}
