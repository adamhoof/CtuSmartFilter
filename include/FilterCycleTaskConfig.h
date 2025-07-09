#pragma once
#include <array>
#include <cstdint>

struct Co2Config
{
    std::array<uint32_t, 5> perLevelMSPeriods = {60000, 120000, 180000, 240000, 300000};
    uint8_t currentLevel = 2;
    float threshold = 1000.0;
};

struct PIDConfig
{
    float p,i,d;
};

struct HeatPadConfig
{
    float targetTemp = 100.0;
    uint32_t regenerationDurationMS = 600000;
    PIDConfig aggressivePID = {10.0, 0.5, 1.0};
    PIDConfig stablePID = {5.0, 0.25, 0.8};
};

struct FilterCycleTaskConfig
{
    Co2Config co2Config;
    HeatPadConfig heatPadConfig;
};
