#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData lastData, double altitude, double heading, double speed, double dt) {
    if (!hasPrevData_) {
        // Initialize with the first valid GPS data
        gpsData_ = lastData;
        lastAltitude_ = altitude;
        lastHeading_ = heading;
        lastSpeed_ = speed;
        hasPrevData_ = true;
    } else {
        if (altitude <= 0.0) return false;

        // Update GPS data based on dead reckoning
        double lat = gpsData_.getLatitude();
        double lon = gpsData_.getLongitude();

        // Simple dead reckoning update (this can be improved with more complex models)
        double distance = speed * dt; // Distance traveled in this time step
        double bearingRad = heading * M_PI / 180.0; // Convert to radians

        // Update latitude and longitude (this is a simplified model)
        lat += (distance * cos(bearingRad)) / 111320.0; // Approximate conversion from meters to degrees
        lon += (distance * sin(bearingRad)) / (111320.0 * cos(lat * M_PI / 180.0)); // Adjust for latitude

        gpsData_.setLatitude(lat);
        gpsData_.setLongitude(lon);
    }

    return true;
}
