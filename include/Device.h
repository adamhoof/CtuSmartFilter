#pragma once

class Device {
public:
    virtual ~Device() = default;
    virtual void init() = 0;
};
