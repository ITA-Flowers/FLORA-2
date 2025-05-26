#pragma once
#include "IDRProcessor.hpp"

class DeadReckoningProcessor : public IDRProcessor {
public:
    DeadReckoningProcessor();

    GPSData getGPSData() const override;

    double getLastAltitude() const { return lastAltitude_; }
    double getLastHeading() const { return lastHeading_; }
    double getLastSpeed() const { return lastSpeed_; }
    bool hasPreviousData() const { return hasPrevData_; }

    bool update(GPSData lastData, double altitude, double heading, double speed) override;

private:
    GPSData gpsData_;
    double lastAltitude_ = 0.0;
    double lastHeading_ = 0.0;
    double lastSpeed_ = 0.0;
    bool hasPrevData_ = false;
};