#pragma once
#include <InputDevice.h>

#include "PWMDevice.h"

class PWMFan : public PWMDevice, public virtual InputDevice {
public:
    PWMFan(const std::string& name, uint8_t pwmPin, uint8_t pwmChannel);

    void init() override;
};
