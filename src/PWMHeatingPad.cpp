#include "PWMHeatingPad.h"
#include <Arduino.h>

PWMHeatingPad::PWMHeatingPad(const char* name, const int8_t pwmPin)
    : PWMControlDevice(name, pwmPin)
{
}

void PWMHeatingPad::init()
{
    pinMode(pwmPin, OUTPUT);
    setPower(0);
}
