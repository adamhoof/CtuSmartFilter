#pragma once
#include <Device.h>
#include <string>

class InputDevice : public Device
{
public:
    explicit InputDevice(std::string deviceName);

    ~InputDevice() override;

    std::string getName() const;

    void init() override;

protected:
    std::string name;
};
