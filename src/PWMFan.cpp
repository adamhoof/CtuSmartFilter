#include "PWMFan.h"
#include <esp32-hal-gpio.h>

PWMFan::PWMFan(const char* name, const uint8_t pwmPin)
    : PWMControlDevice(name, pwmPin)
{
}

void PWMFan::init()
{
    pinMode(pwmPin, OUTPUT);
    setPower(0);
}

void PWMFan::runAtIdle()
{
    setPower(30);
}

void PWMFan::runAtMax()
{
    setPower(100);
}

void PWMFan::turnOff()
{
    setPower(0);
}
