#include "I2CScanner.h"

#include <iomanip>
#include <sstream>
#include <Wire.h>
#include <string>

I2CScanner::I2CScanner() = default;

void I2CScanner::addDevice(I2CDevice& i2cDevice) {
    i2cDevices.emplace_back(i2cDevice);
}

void I2CScanner::addDevices(const std::vector<std::reference_wrapper<I2CDevice>>& devicesList) {
    i2cDevices.insert(i2cDevices.end(), devicesList.begin(), devicesList.end());
}

std::vector<I2CScanner::ScanResult> I2CScanner::scan() const
{
    std::vector<ScanResult> results;

    for (const auto& device : i2cDevices) {
        const byte address = device.get().getAddress();
        const std::string deviceName = device.get().getName();

        std::stringstream hexAddressStream;
        hexAddressStream << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                         << static_cast<int>(address);
        std::string hexAddress = hexAddressStream.str();

        Wire.beginTransmission(address);
        delay(5);
        const byte error = Wire.endTransmission();

        if (error == 0) {
            results.push_back({
                SUCCESS,
                "Device '" + deviceName + "' found at address " += hexAddress
            });
        } else if (error == 4) {
            results.push_back({
                FAILURE,
                "Unknown error with device '" + deviceName +
                "' at address " += hexAddress
            });
        } else {
            results.push_back({
                FAILURE,
                "Device '" + deviceName + "' at address " += hexAddress +
                " did not respond."
            });
        }
        delay(100);
    }

    return results;
}


