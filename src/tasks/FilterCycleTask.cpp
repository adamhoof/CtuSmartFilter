#include "tasks/FilterCycleTask.h"

bool isCo2LevelAboveThreshold(const double co2Val, const double co2Threshold)
{
    return co2Val > co2Threshold;
}

void regulateHeatingPad(const FilterRegenTaskParams* p)
{
    unsigned int heatingPadDuration = pdMS_TO_TICKS(p->conf.heatingPadDurationMs);
    p->heatingPad.setPower(100);
    // TODO first heatup pad completely to 100 deg C, then PID

    unsigned int startTime = xTaskGetTickCount();
    while (xTaskGetTickCount() - startTime < heatingPadDuration) {
        Measurement temperatureMeasurement = Measurement{};
        double currentTemp = temperatureMeasurement.value;

        // TODO PID instead
        if (currentTemp < p->conf.heatingPadTargetTemp) {
            p->heatingPad.setPower(100);
        }
        else {
            p->heatingPad.setPower(50);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    p->heatingPad.setPower(0);
}

void regenerateFilter(const FilterRegenTaskParams* p)
{
    p->fan.turnOff();
    regulateHeatingPad(p);
}

void regulateCO2(const FilterRegenTaskParams* p)
{
    // first comes the regulation for the specified interval, which always happens
    const TickType_t periodStartTime = xTaskGetTickCount();
    const TickType_t periodDuration = pdMS_TO_TICKS(p->conf.co2LevelPeriods[p->conf.currentCO2Level - 1]);

    while (xTaskGetTickCount() - periodStartTime < periodDuration) {
        Serial.printf("Is CO2 high?: %d, value: %f\n",
                      isCo2LevelAboveThreshold(p->co2Sensor.performMeasurement().value,
                                               p->conf.co2Threshold), p->co2Sensor.performMeasurement().value);

        isCo2LevelAboveThreshold(p->co2Sensor.performMeasurement().value, p->conf.co2Threshold)
            ? p->fan.runAtMax()
            : p->fan.runAtIdle();

        vTaskDelay(pdMS_TO_TICKS(8000));
    }

    // second comes the regulation that makes sure the CO2 is acceptable
    p->fan.runAtMax();
    while (isCo2LevelAboveThreshold(p->co2Sensor.performMeasurement().value, p->conf.co2Threshold)) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    p->fan.runAtIdle();
}

void filterCycleTask(void* parameter)
{
    const auto* p = static_cast<FilterRegenTaskParams*>(parameter);

    // TODO once critical sensor becomes unreliable, halt the operation
    while (true) {
        Serial.printf("Regulate CO2\n");
        regulateCO2(p);
        Serial.printf("Regenerate filter\n");
        regenerateFilter(p);
    }
}
