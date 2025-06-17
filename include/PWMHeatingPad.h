#pragma once
#include <PWMControlDevice.h>

class PWMHeatingPad : public PWMControlDevice
{
public:
    PWMHeatingPad(const char* name, int8_t pwmPin);

    void init() override;

    ~PWMHeatingPad() override = default;
};
