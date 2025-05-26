#include "DeadReckoningProcessor.hpp"

DeadReckoningProcessor::DeadReckoningProcessor() {}

GPSData DeadReckoningProcessor::getGPSData() const {
    return gpsData_;
}

bool DeadReckoningProcessor::update(GPSData initialGpsData, double altitude, double headingDeg, double speed, double dt) {
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
        return true;
    }

    if (altitude <= 0.0) return false;

    constexpr double RM = 6367449.0;
    constexpr double RE = 6378137.0; 

    double lat = gpsData_.getLatitude();    
    double lon = gpsData_.getLongitude();  
    double distance = speed * dt;        
    double headingRad = headingDeg * M_PI / 180.0;

    double Ds = (2.0 * M_PI * RM / 360.0) * lat;               
    double Dd = (2.0 * M_PI * RE / 360.0) * ((90.0 - lat) / 90.0);

    lat += (distance / Ds) * cos(headingRad);
    lon += (distance / Dd) * sin(headingRad);

    gpsData_.setLatitude(lat);
    gpsData_.setLongitude(lon);
    lastAltitude_ = altitude;
    lastHeading_ = headingDeg;
    lastSpeed_ = speed;

    return true;
}

