#pragma once

using std::string;

using MeasurementName = std::string;
using Unit = std::string;

struct Measurement {

    MeasurementName name;
    double value;
    Unit unit;
};