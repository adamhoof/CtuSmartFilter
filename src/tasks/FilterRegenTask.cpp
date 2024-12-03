#include "tasks/FilterRegenTask.h"
#include <freertos/task.h>

namespace
{
    bool isCo2LevelHigh(const CO2Sensor& co2Sensor, const uint32_t co2Threshold)
    {
        const Measurement co2Measurement = co2Sensor.getCO2Value();
        return co2Measurement.value > co2Threshold;
    }

    void regulateHeatingPad(const FilterRegenTaskParams* params)
    {
        TickType_t heatingPadDuration = pdMS_TO_TICKS(params->config.heatingPadDurationMs);
        TickType_t startTime = xTaskGetTickCount();
        params->heatingPad.setPower(255);

        while (xTaskGetTickCount() - startTime < heatingPadDuration) {
            Measurement temperatureMeasurement = params->thermocoupleSensor.getTemperatureValue();
            double currentTemp = temperatureMeasurement.value;

            if (currentTemp < params->config.heatingPadTargetTemp) {
                params->heatingPad.setPower(255);
            }
            else {
                params->heatingPad.setPower(128);
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        params->heatingPad.setPower(0);
    }

    void regenerateFilter(const FilterRegenTaskParams* params)
    {
        params->fan.turnOff();
        regulateHeatingPad(params);
    }
}

void filterRegenTask(void* parameter)
{
    const auto* params = static_cast<FilterRegenTaskParams*>(parameter);

    while (true) {
        const TickType_t periodDuration = pdMS_TO_TICKS(params->config.levelPeriods[params->config.currentLevel - 1]);
        const TickType_t periodStartTime = xTaskGetTickCount();

        while (xTaskGetTickCount() - periodStartTime < periodDuration) {
            const bool co2IsHigh = isCo2LevelHigh(params->co2Sensor, params->config.co2Threshold);

            if (co2IsHigh) {
                params->fan.runAtMax();
            }
            else {
                params->fan.runAtIdle();
            }

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        if (!isCo2LevelHigh(params->co2Sensor, params->config.co2Threshold)) {
            regenerateFilter(params);
        }
    }
}
