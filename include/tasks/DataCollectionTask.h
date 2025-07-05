#pragma once
#include "SensorDevice.h"
#include <vector>

#include "SensorDataBank.h"

struct DataCollectionTaskParams {
  std::vector<SensorDevice*> sensorsToCollectMeasurementsFrom;
  SensorDataBank& sensorDataBank;
};

void dataCollectionTask(void* parameter);
