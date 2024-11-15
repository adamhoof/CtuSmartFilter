#include "I2CScanner.h"
#include <Arduino.h>
#include <Wire.h>
#include <functional>

I2CScanner::I2CScanner() = default;

void I2CScanner::addDevice(I2CDevice& i2cDevice)
{
    i2cDevices.emplace_back(i2cDevice);
}

void I2CScanner::addDevices(const std::vector<std::reference_wrapper<I2CDevice>>& devicesList) {
    i2cDevices.insert(i2cDevices.end(), devicesList.begin(), devicesList.end());
}

void I2CScanner::scan()
{
    Serial.println("Scanning...");
    int nDevices = 0;
    for (const auto& device: i2cDevices) {
        const byte address = device.get().getAddress();
        const std::string deviceName = device.get().getName();

        Wire.beginTransmission(address);
        delay(5);
        const byte error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found: ");
            Serial.print(deviceName.c_str());
            Serial.print(" at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" - Good!");
            nDevices++;
        }
        else if (error == 4) {
            Serial.print("Unknown error with device: ");
            Serial.print(deviceName.c_str());
            Serial.print(" at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" - Bad! Device did not respond.");
        }
        else {
            Serial.print("Device: ");
            Serial.print(deviceName.c_str());
            Serial.print(" at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" - Bad! Device did not respond.");
        }
        delay(100);
    }

    if (nDevices == 0) {
        Serial.println("No I2C devices found");
    }
    else {
        Serial.println("Scan complete");
    }

    delay(5000);
}
