#include "PWMHeatingPad.h"
#include <Arduino.h>

PWMHeatingPad::PWMHeatingPad(const std::string& name, const int8_t pwmPin)
    : InputDevice(name), PWMDevice(pwmPin)
{
}

void PWMHeatingPad::init()
{
    pinMode(pwmPin, OUTPUT);
    setPower(0);
}
