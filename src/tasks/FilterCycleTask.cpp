#include "tasks/FilterCycleTask.h"
#include "QuickPID.h"

class PIDController
{
public:
    PIDController(float* input, float* output, float* setpoint,
                  const uint32_t sampleTimeMs)
        : quickPID(input, output, setpoint)
    {
        quickPID.SetProportionalMode(QuickPID::pMode::pOnMeas);
        quickPID.SetDerivativeMode(QuickPID::dMode::dOnMeas);
        quickPID.SetAntiWindupMode(QuickPID::iAwMode::iAwCondition);
        quickPID.SetOutputLimits(0, 100);
        quickPID.SetSampleTimeUs(sampleTimeMs * 1000);
    }

    void switchStrategy(const PIDConfig& pid)
    {
        quickPID.SetTunings(pid.p, pid.i, pid.d);
        quickPID.Initialize();
    }

    bool compute() { return quickPID.Compute(); }

    void setManual() { quickPID.SetMode(QuickPID::Control::manual); }

    void setAutomatic() { quickPID.SetMode(QuickPID::Control::automatic); }

    uint8_t getMode() const { return const_cast<QuickPID&>(quickPID).GetMode(); }

public:
    QuickPID quickPID;
};

void haltOperation(PIDController& pid, const FilterCycleTaskParams* p)
{
    pid.setManual();
    p->heatingPad.setPower(0);
    Serial.println("ERROR: Thermocouple unreliable. Halting regeneration cycle.");
}

void pauseOperation(PIDController& pid, const FilterCycleTaskParams* p)
{
    pid.setManual();
    p->heatingPad.setPower(0);
    Serial.println("WARNING: Pausing due to temporary sensor read error.");
}

void resumeOperation(PIDController& pid)
{
    if (pid.getMode() == static_cast<uint8_t>(QuickPID::Control::manual)) {
        pid.setAutomatic();
    }
}

bool runPidIteration(PIDController& pid, const FilterCycleTaskParams* p, float& currentTemp)
{
    if (!p->filterThermocoupleSensor.isReliable()) {
        haltOperation(pid, p);
        return false;
    }

    const Measurement m = p->filterThermocoupleSensor.performMeasurement();
    if (strcmp(m.statusMessage.data(), "OK") != 0) {
        pauseOperation(pid, p);
        return true;
    }

    resumeOperation(pid);
    currentTemp = m.value;

    Serial.printf("Current temp %f -> Before: %f ", currentTemp, pid.quickPID.outputSum);
    pid.compute();
    p->heatingPad.setPower(static_cast<uint8_t>(pid.quickPID.outputSum));
    Serial.printf(" After: %f\n", pid.quickPID.outputSum);

    return true;
}

bool regenerateFilter(const FilterCycleTaskParams* p)
{
    p->fan.turnOff();

    float currentTemp = 0.0f;
    float pidOutput = 0.0f;
    float targetTemp = p->c.heatPadConfig.targetTemp;

    PIDController pidController(&currentTemp, &pidOutput, &targetTemp, p->filterThermocoupleSensor.getMeasurementRefreshMS());

    pidController.switchStrategy(p->c.heatPadConfig.aggressivePID);
    pidController.setAutomatic();

    // TODO run max for 2 mins, if cant heatup, send ERROR
    while (true) {
        if (!runPidIteration(pidController, p, currentTemp)) {
            return false;
        }

        if (currentTemp >= targetTemp) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(p->filterThermocoupleSensor.getMeasurementRefreshMS()));
    }

    pidController.switchStrategy(p->c.heatPadConfig.stablePID);
    pidController.setAutomatic();

    const unsigned int startTime = xTaskGetTickCount();
    const TickType_t regenerationDurationTicks = pdMS_TO_TICKS(p->c.heatPadConfig.regenerationDurationMS);

    while (xTaskGetTickCount() - startTime < regenerationDurationTicks) {
        if (!runPidIteration(pidController, p, currentTemp)) {
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(p->filterThermocoupleSensor.getMeasurementRefreshMS()));
    }

    p->heatingPad.setPower(0);
    return true;
}

bool isCo2LevelAboveThreshold(const double co2Val, const double co2Threshold)
{
    return co2Val > co2Threshold;
}

bool regulateCO2(const FilterCycleTaskParams* p)
{
    // first comes the regulation for the specified interval, which always happens
    const TickType_t periodStartTime = xTaskGetTickCount();
    const TickType_t periodDuration = pdMS_TO_TICKS(p->c.co2Config.perLevelMSPeriods[p->c.co2Config.currentLevel - 1]);

    while (xTaskGetTickCount() - periodStartTime < periodDuration) {
        if (!p->roomCo2Sensor.isReliable()) {
            p->fan.turnOff();
            return false;
        }

        Measurement m = p->roomCo2Sensor.performMeasurement();
        if (strcmp(m.statusMessage.data(), "OK") != 0) {
            p->fan.runAtIdle();
            vTaskDelay(pdMS_TO_TICKS(p->roomCo2Sensor.getMeasurementRefreshMS()));
            continue;
        }

        isCo2LevelAboveThreshold(m.value, p->c.co2Config.threshold)
            ? p->fan.runAtMax()
            : p->fan.runAtIdle();
        vTaskDelay(pdMS_TO_TICKS(p->roomCo2Sensor.getMeasurementRefreshMS()));
    }

    // second comes the regulation that makes sure the CO2 is acceptable before regenerating the filter
    // TODO this could run forever
    while (true) {
        if (!p->roomCo2Sensor.isReliable()) {
            p->fan.turnOff();
            return false;
        }

        Measurement m = p->roomCo2Sensor.performMeasurement();
        if (strcmp(m.statusMessage.data(), "OK") != 0) {
            p->fan.runAtIdle();
            vTaskDelay(pdMS_TO_TICKS(p->roomCo2Sensor.getMeasurementRefreshMS()));
            continue;
        }

        if (isCo2LevelAboveThreshold(m.value, p->c.co2Config.threshold)) {
            p->fan.runAtMax();
            vTaskDelay(pdMS_TO_TICKS(p->roomCo2Sensor.getMeasurementRefreshMS()));
        } else {
            p->fan.turnOff();
            break;
        }
    }

    return true;
}

[[noreturn]] void filterCycleTask(void* params)
{
    const auto* p = static_cast<FilterCycleTaskParams*>(params);

    while (true) {
        Serial.printf("Regulate CO2\n");
        if (!regulateCO2(p)) {
            Serial.println("Failed to regulate CO2. Restarting.");
            //TODO take lock or whatever
            vTaskDelay(portMAX_DELAY);
        };
        Serial.printf("Regenerate filter\n");
        if (!regenerateFilter(p)) {
            Serial.println("Failed to regenerate filter. Restarting.");
            vTaskDelay(portMAX_DELAY);
        };
    }
}
