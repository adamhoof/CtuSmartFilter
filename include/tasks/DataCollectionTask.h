#pragma once
#include "SensorDevice.h"
#include <vector>

struct DataCollectionTaskParams {
  std::vector<SensorDevice*> sensors;
};

void dataCollectionTask(void* params);
