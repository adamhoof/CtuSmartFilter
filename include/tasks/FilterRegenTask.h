#pragma once

#include <CO2Sensor.h>
#include <vector>
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "ThermocoupleSensor.h"

#define DefaultFilterRegenTaskParams nullptr

struct FilterRegenTaskConfig
{
    std::array<uint32_t, 5> levelPeriods = {60000, 120000, 180000, 240000, 300000};
    uint8_t currentLevel = 2;
    int co2Threshold = 1000;
    uint8_t heatingPadTargetTemp = 100;
    uint32_t heatingPadDurationMs = 600000;
};

struct FilterRegenTaskParams
{
    CO2Sensor& co2Sensor;
    PWMFan& fan;
    PWMHeatingPad& heatingPad;
    ThermocoupleSensor& thermocoupleSensor;
    FilterRegenTaskConfig* config;
};

void filterRegenTask(void* parameter);
