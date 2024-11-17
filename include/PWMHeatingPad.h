#pragma once
#include <InputDevice.h>
#include <PWMDevice.h>

class PWMHeatingPad : public PWMDevice, public virtual InputDevice
{
public:
    PWMHeatingPad(const std::string& name, int8_t pwmPin, int8_t pwmChannel);

    void init() override;

    ~PWMHeatingPad() override;
};
