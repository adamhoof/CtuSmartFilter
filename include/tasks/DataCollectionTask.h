#pragma once
#include "SensorDevice.h"
#include <vector>

struct MeasurementsPerformingTaskParams {
  std::vector<SensorDevice*> sensorsToCollectMeasurementsFrom;
};

void measurementsPerformingTask(void* parameter);
