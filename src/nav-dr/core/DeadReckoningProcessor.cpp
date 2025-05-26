#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData lastData, double altitude, double heading, double speed) {
    if (speed <= 0.0 || altitude <= 0.0) return false;

    double distance = speed * 1.0;
    double deltaX = distance * std::cos(heading * M_PI / 180.0);
    double deltaY = distance * std::sin(heading * M_PI / 180.0);

    return true;
}
