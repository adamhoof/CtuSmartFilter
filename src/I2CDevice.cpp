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

CommunicationAttemptResult I2CDevice::performTemplateCommunicationTest(
    const char* name, const byte address) const {

    Wire.beginTransmission(address);
    delay(5);
    const byte error = Wire.endTransmission();

    static char messageBuffer[96];
    const char* format;

    if (error == 0) {
        format = "Device '%s' is responding at address 0x%02X";
        snprintf(messageBuffer, sizeof(messageBuffer), format, name, address);
        return {SUCCESS, messageBuffer};
    }

    format = "Device '%s' is not responding at address 0x%02X";
    snprintf(messageBuffer, sizeof(messageBuffer), format, name, address);
    return {FAILURE, messageBuffer};
}
