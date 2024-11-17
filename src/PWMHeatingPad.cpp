#include "PWMHeatingPad.h"

#include <HardwareSerial.h>

PWMHeatingPad::PWMHeatingPad(const std::string& name, const int8_t pwmPin,
                             const int8_t pwmChannel) : PWMDevice(pwmPin, pwmChannel),
                                                        InputDevice(name)
{
}

void PWMHeatingPad::init()
{
    ledcSetup(pwmChannel, 25000, 8);
    ledcAttachPin(pwmPin, pwmChannel);
    setPower(0);
}
