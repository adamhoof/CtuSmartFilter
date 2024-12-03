#pragma once

#include <CO2Sensor.h>
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "ThermocoupleSensor.h"

struct FilterRegenTaskConfig
{
    std::vector<uint32_t> levelPeriods = {60000, 120000, 180000, 240000, 300000};
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
    const FilterRegenTaskConfig& config;
};

void filterRegenTask(void* parameter);
