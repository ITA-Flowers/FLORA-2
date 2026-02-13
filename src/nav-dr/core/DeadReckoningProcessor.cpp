#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData initialGpsData, double altitude, double heading, double speed, double dt) {
    const double HEADING_CORRECTION_DEG = 90.0;

    if (!hasPrevData_) {
        if (initialGpsData.getLatitude() < 1.0 || initialGpsData.getLongitude() < 1.0) {
            std::cerr << "Error: Initial GPS fix is invalid." << std::endl;
            return false;
        }

        originGpsData_ = initialGpsData;
        gpsData_ = originGpsData_;
        enuPosition_ = Vector3D(0.0, 0.0, 0.0);
        lastAltitude_ = altitude;
        lastHeading_ = heading;
        lastSpeed_ = speed;
        hasPrevData_ = true;
    } else {
        if (altitude <= 0.0) return false;

        double correctedHeading = -heading * 180.0 / M_PI + HEADING_CORRECTION_DEG;
        double bearingRad = correctedHeading * M_PI / 180.0; // [rad], clockwise from North

        double distance = speed * dt;
        double dEast = distance * std::sin(bearingRad);
        double dNorth = distance * std::cos(bearingRad);
        double dUp = altitude - lastAltitude_;

        enuPosition_.setX(enuPosition_.getX() + dEast);
        enuPosition_.setY(enuPosition_.getY() + dNorth);
        enuPosition_.setZ(enuPosition_.getZ() + dUp);

        gpsData_.fromENU(
            enuPosition_,
            originGpsData_.getLatitude(),
            originGpsData_.getLongitude(),
            originGpsData_.getAltitude());
        gpsData_.setAltitude(originGpsData_.getAltitude() + enuPosition_.getZ());

        lastAltitude_ = altitude;
        lastHeading_ = heading;
        lastSpeed_ = speed;
    }


    return true;
}

