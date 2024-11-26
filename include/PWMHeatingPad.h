#pragma once
#include <InputDevice.h>
#include <PWMDevice.h>

class PWMHeatingPad : public PWMDevice, public virtual InputDevice
{
public:
    PWMHeatingPad(const std::string& name, int8_t pwmPin);

    void init() override;

    ~PWMHeatingPad() override = default;
};
