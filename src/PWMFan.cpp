#include "PWMFan.h"
#include <esp32-hal-gpio.h>

PWMFan::PWMFan(const std::string& name, const uint8_t pwmPin)
    : InputDevice(name), PWMDevice(pwmPin)
{
}

void PWMFan::init()
{
    pinMode(pwmPin, OUTPUT);
    setPower(0);
}
