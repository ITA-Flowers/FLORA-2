#pragma once
#include "../sensors/SensorData.hpp"

class IDRProcessor {
public:
    virtual ~IDRProcessor() = default;
  
    virtual GPSData getGPSData() const = 0;

    virtual bool update(GPSData lastData, double altitude, double heading, double speed, double dt) = 0;
};