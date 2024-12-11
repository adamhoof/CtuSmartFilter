#include "tasks/FilterRegenTask.h"

bool isCo2LevelHigh(const CO2Sensor& co2Sensor, const uint32_t co2Threshold)
{
    const Measurement co2Measurement = co2Sensor.getCO2Value();
    return co2Measurement.value > co2Threshold;
}

void regulateHeatingPad(const FilterRegenTaskParams* params)
{
    unsigned int heatingPadDuration = pdMS_TO_TICKS(params->config.heatingPadDurationMs);
    unsigned int startTime = xTaskGetTickCount();
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

void waitUntilCO2IsAcceptable(const FilterRegenTaskParams* params)
{
    params->fan.runAtMax();
    while (isCo2LevelHigh(params->co2Sensor, params->config.heatingPadDurationMs)) {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void filterRegenTask(void* parameter)
{
    const auto* params = static_cast<FilterRegenTaskParams*>(parameter);
    //TODO make this react to changes in state?
    const TickType_t periodDuration = pdMS_TO_TICKS(params->config.levelPeriods[params->config.currentLevel - 1]);

    while (true) {
        const TickType_t periodStartTime = xTaskGetTickCount();

        while (xTaskGetTickCount() - periodStartTime < periodDuration) {
            isCo2LevelHigh(params->co2Sensor, params->config.co2Threshold)
                ? params->fan.runAtMax()
                : params->fan.runAtIdle();

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        if (isCo2LevelHigh(params->co2Sensor, params->config.co2Threshold)) {
            waitUntilCO2IsAcceptable(params);
        }

        regenerateFilter(params);
    }
}
