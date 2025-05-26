#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData initialGpsData, double altitude, double heading, double speed, double dt) {
    if (!hasPrevData_) {
        if (initialGpsData.getLatitude() < 1.0 || initialGpsData.getLongitude() < 1.0) {
            std::cerr << "Error: Initial GPS fix is invalid." << std::endl;
            return false;
        }
        gpsData_ = initialGpsData;
        lastAltitude_ = altitude;
        lastHeading_ = heading;
        lastSpeed_ = speed;
        hasPrevData_ = true;
        return true;
    }

    if (altitude <= 0.0) return false;

    // Get previous coordinates
    double lat = gpsData_.getLatitude();
    double lon = gpsData_.getLongitude();

    // Compute distance and heading
    double distance = speed * dt; // [m]
    double bearingRad = heading * M_PI / 180.0; // [rad]
    double latRad = lat * M_PI / 180.0;

    // Convert to degrees (approx.)
    constexpr double R = 6371000.0; // Earth radius in meters

    double deltaLat = (distance / R * cos(bearingRad)) / 111320.0;
    double deltaLon = (distance / R * sin(bearingRad)) / (111320.0 * cos(latRad));

    // Apply updates
    gpsData_.setLatitude(lat + deltaLat * 180.0 / M_PI);
    gpsData_.setLongitude(lon + deltaLon * 180.0 / M_PI);

    // Update internal state
    lastAltitude_ = altitude;
    lastHeading_ = heading;
    lastSpeed_ = speed;

    return true;
}

