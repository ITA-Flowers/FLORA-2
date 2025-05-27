#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData initialGpsData, double altitude, double heading, double speed, double dt) {
    const double R = 6378137.0;
    const double HEADING_CORRECTION_DEG = 90.0;

    if (!hasPrevData_) {
        if (initialGpsData.getLatitude() < 1.0 || initialGpsData.getLongitude() < 1.0) {
            std::cerr << "Error: Initial GPS fix is invalid." << std::endl;
            return false;
        }

        gpsData_ = initialGpsData;
        lastAltitude_ = altitude;
        lastHeading_ = headingDeg;
        lastSpeed_ = speed;
        hasPrevData_ = true;
    } else {
        if (altitude <= 0.0) return false;

        double correctedHeading = -heading * 180.0 / M_PI + HEADING_CORRECTION_DEG;
        double bearingRad = correctedHeading * M_PI / 180.0;

        double distance = speed * dt;

        double lat1 = gpsData_.getLatitude() * M_PI / 180.0;
        double lon1 = gpsData_.getLongitude() * M_PI / 180.0;

        double lat2 = std::asin(std::sin(lat1) * std::cos(distance / R) +
                                std::cos(lat1) * std::sin(distance / R) * std::cos(bearingRad));

        double lon2 = lon1 + std::atan2(std::sin(bearingRad) * std::sin(distance / R) * std::cos(lat1),
                                        std::cos(distance / R) - std::sin(lat1) * std::sin(lat2));

        gpsData_.setLatitude(lat2 * 180.0 / M_PI);
        gpsData_.setLongitude(lon2 * 180.0 / M_PI);

        lastAltitude_ = altitude;
        lastHeading_ = heading;
        lastSpeed_ = speed;
    }


    return true;
}

