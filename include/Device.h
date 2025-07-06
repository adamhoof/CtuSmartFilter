#pragma once

class Device {
public:
    explicit Device(const char* deviceName);
    virtual ~Device() = default;
    virtual void init() = 0;
    const char* getName() const;
private:
    const char* deviceName;
};
