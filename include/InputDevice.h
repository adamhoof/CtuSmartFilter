#pragma once
#include <Device.h>
#include <string>

class InputDevice : public Device
{
public:
    explicit InputDevice(std::string deviceName);

    ~InputDevice() override = default;

    std::string getName() const;

protected:
    std::string name;
};
