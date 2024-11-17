#pragma once
#include <Arduino.h>
#include <CommunicationTestable.h>

class I2CDevice
{
public:
    explicit I2CDevice(byte address);

    virtual ~I2CDevice() = default;

    virtual byte getAddress() const;

    CommunicationAttemptResult performTemplateCommunicationTest(const std::string& name, byte address) const;

protected:
    byte address;
};
