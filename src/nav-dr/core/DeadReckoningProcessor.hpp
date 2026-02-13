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
    Vector3D getENUPosition() const { return enuPosition_; }

    bool update(GPSData initialGpsData, double altitude, double heading, double speed, double dt) override;

private:
    GPSData originGpsData_;
    GPSData gpsData_;
    Vector3D enuPosition_ = Vector3D(0.0, 0.0, 0.0); // East, North, Up [m]
    double lastAltitude_ = 0.0;
    double lastHeading_ = 0.0;
    double lastSpeed_ = 0.0;
    bool hasPrevData_ = false;
};
