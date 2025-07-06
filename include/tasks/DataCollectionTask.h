#pragma once
#include "SensorDevice.h"
#include <vector>

struct DataCollectionTaskParams {
  std::vector<SensorDevice*> sensorsToCollectMeasurementsFrom;
};

void dataCollectionTask(void* parameter);
