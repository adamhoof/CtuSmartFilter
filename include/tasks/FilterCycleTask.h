#pragma once
#include "CO2Sensor.h"
#include "PWMFan.h"
#include "PWMHeatingPad.h"
#include "ThermocoupleSensor.h"

#include "FilterCycleTaskConfig.h"

struct FilterCycleTaskParams
{
    PWMFan& fan;
    PWMHeatingPad& heatingPad;
    CO2Sensor& roomCo2Sensor;
    ThermocoupleSensor& filterThermocoupleSensor;
    FilterCycleTaskConfig c;
};

void filterCycleTask(void* params);
