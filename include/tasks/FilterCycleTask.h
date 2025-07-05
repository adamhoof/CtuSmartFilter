#pragma once

#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "SensorDataBank.h"

struct FilterRegenTaskConfig
{
    std::array<uint32_t, 5> co2LevelPeriods = {60000, 120000, 180000, 240000, 300000};
    uint8_t currentCO2Level = 2;
    double co2Threshold = 1000.0;
    uint8_t heatingPadTargetTemp = 100;
    uint32_t heatingPadDurationMs = 600000;
};

struct FilterRegenTaskParams
{
    PWMFan& fan;
    PWMHeatingPad& heatingPad;
    SensorDataBank& sensorDataBank;
    FilterRegenTaskConfig conf;
};

void filterCycleTask(void* parameter);
