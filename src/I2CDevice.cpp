#include "I2CDevice.h"

#include <Wire.h>

I2CDevice::I2CDevice(const byte address)
    :address(address)
{
}

byte I2CDevice::getAddress() const
{
    return address;
}

CommunicationAttemptResult I2CDevice::performTemplateCommunicationTest(const std::string& name, const byte address) const
{
    Wire.beginTransmission(address);
    delay(5);
    const byte error = Wire.endTransmission();

    if (error == 0) {
        return {
            SUCCESS, "Device '" + name + "' is responding at address 0x" +
                     String(address, HEX).c_str()
        };
    }

    return {
        FAILURE, "Device '" + name + "' is not responding at address 0x" +
                 String(address, HEX).c_str()
    };
}
