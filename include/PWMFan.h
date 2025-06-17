#pragma once

#include "PWMControlDevice.h"

class PWMFan : public PWMControlDevice {
public:
    explicit PWMFan(const char* name, uint8_t pwmPin);

    void init() override;

    void runAtIdle();

    void runAtMax();

    void turnOff();
};
