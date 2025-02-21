#include "tasks/FilterRegenTask.h"

#include <InvalidValue.h>

bool isCo2LevelAboveThreshold(const CO2Sensor& co2Sensor, const uint32_t co2Threshold)
{
    const Measurement co2Measurement = co2Sensor.getCO2Value();
    if (co2Measurement.value == INVALID_VALUE) {
        return true;
    }
    Serial.printf("Val: %f\n",co2Measurement.value);
    Serial.printf("Thresh: %d\n",co2Threshold);
    return co2Measurement.value > co2Threshold;
}

void regulateHeatingPad(const FilterRegenTaskParams* params, FilterRegenTaskConfig& conf)
{
    unsigned int heatingPadDuration = pdMS_TO_TICKS(conf.heatingPadDurationMs);
    unsigned int startTime = xTaskGetTickCount();
    params->heatingPad.setPower(255);

    while (xTaskGetTickCount() - startTime < heatingPadDuration) {
        Measurement temperatureMeasurement = params->thermocoupleSensor.getTemperatureValue();
        double currentTemp = temperatureMeasurement.value;

        if (currentTemp < conf.heatingPadTargetTemp) {
            params->heatingPad.setPower(255);
        }
        else {
            params->heatingPad.setPower(128);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    params->heatingPad.setPower(0);
}

void regenerateFilter(const FilterRegenTaskParams* params, FilterRegenTaskConfig& conf)
{
    params->fan.turnOff();
    regulateHeatingPad(params, conf);
}

void waitUntilCO2IsAcceptable(const CO2Sensor& co2Sensor, const uint32_t co2Threshold)
{
    while (isCo2LevelAboveThreshold(co2Sensor,co2Threshold)) {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void filterRegenTask(void* parameter)
{
    const auto* params = static_cast<FilterRegenTaskParams*>(parameter);

    FilterRegenTaskConfig conf{};
    if (params->config != nullptr) {
        conf = *(params->config);
    }
    //TODO make this react to changes in state?
    const TickType_t periodDuration = pdMS_TO_TICKS(conf.levelPeriods[conf.currentLevel - 1]);

    while (true) {
        Serial.println("Entering cycle of while true");
        const TickType_t periodStartTime = xTaskGetTickCount();
        Serial.printf("Tick count: %d\n", periodStartTime);

        while (xTaskGetTickCount() - periodStartTime < periodDuration) {
            Serial.printf("Is CO2 high?: %d, value: %f\n", isCo2LevelAboveThreshold(params->co2Sensor, conf.co2Threshold), params->co2Sensor.getCO2Value().value);
            isCo2LevelAboveThreshold(params->co2Sensor, conf.co2Threshold)
                ? params->fan.runAtMax()
                : params->fan.runAtIdle();

            vTaskDelay(pdMS_TO_TICKS(3000));
        }

        Serial.printf("Is CO2 high before regen?: %d\n", isCo2LevelAboveThreshold(params->co2Sensor, conf.co2Threshold));
        if (isCo2LevelAboveThreshold(params->co2Sensor, conf.co2Threshold)) {
            Serial.printf("Waiting for CO2 to lower down\n");
            params->fan.runAtMax();
            waitUntilCO2IsAcceptable(params->co2Sensor, conf.co2Threshold);
        }

        Serial.printf("Proceeding to regenerate filter\n");
        regenerateFilter(params, conf);
    }
}
