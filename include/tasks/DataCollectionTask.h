#pragma once

#include "OutputDevice.h"

struct MeasurementsPerformingTaskParams {
  std::vector<std::reference_wrapper<OutputDevice>> devicesToCollectMeasurementsFrom;
};

void measurementsPerformingTask(void* parameter);
