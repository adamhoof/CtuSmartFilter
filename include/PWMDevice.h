#pragma once
#include <cstdint>

class PWMDevice{
public:
    PWMDevice(uint8_t pwmPin);

    void increasePower(uint8_t value);
    void decreasePower(uint8_t value);
    void setPower(uint8_t value);
    uint8_t getPower() const;

protected:
    uint8_t pwmPin;
    uint8_t powerLevel;
};
