#pragma once
#include "SensorDevice.h"
#include <vector>

#include "SensorDataBank.h"

struct MeasurementsPerformingTaskParams {
  std::vector<SensorDevice*> sensorsToCollectMeasurementsFrom;
  SensorDataBank& sensorDataBank;
};

void dataCollectionTask(void* parameter);
