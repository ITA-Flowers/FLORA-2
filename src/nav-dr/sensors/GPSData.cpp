// GPSData.cpp
#include "GPSData.hpp"
#include <cmath>


// Earth radius in meters
const double EARTH_RADIUS = 6371000.0;
// Convert degrees to radians
inline double deg2rad(double deg) { return deg * M_PI / 180.0; }
// Convert radians to degrees
inline double rad2deg(double rad) { return rad * 180.0 / M_PI; }

GPSData::GPSData()
    : latitude(0.0)
    , longitude(0.0)
    , altitude(0.0)
    , accuracy(0.0)
    , fixType(FixType::NO_FIX)
    , satelliteCount(0)
{
}

GPSData::GPSData(double latitude, double longitude, double altitude, 
                double accuracy, FixType fixType, int satelliteCount)
    : latitude(latitude)
    , longitude(longitude)
    , altitude(altitude)
    , accuracy(accuracy)
    , fixType(fixType)
    , satelliteCount(satelliteCount)
{
}

bool GPSData::isValid() const {
    // Check latitude range (-90 to 90 degrees)
    if (latitude < -90.0 || latitude > 90.0) {
        return false;
    }
    
    // Check longitude range (-180 to 180 degrees)
    if (longitude < -180.0 || longitude > 180.0) {
        return false;
    }
    
    // Check altitude (Dead Sea is -430m, Mt. Everest is 8,848m, add margins)
    if (altitude < -500.0 || altitude > 9000.0) {
        return false;
    }
    
    // Check fix type (must not be NO_FIX)
    if (fixType == FixType::NO_FIX) {
        return false;
    }
    
    // Check if we have at least 3 satellites for a minimal fix
    if (satelliteCount < 3) {
        return false;
    }
    
    // All checks passed
    return true;
}

Vector3D GPSData::toENU(double referenceLatitude, double referenceLongitude, double referenceAltitude) const {
    // Użyj long double dla wyższej precyzji
    const long double DEG_TO_RAD = M_PI / 180.0L;
    
    // Dokładniejsze parametry elipsoidy WGS84
    const long double a = 6378137.0L;          
    const long double f = 1.0L / 298.257223563L; 
    const long double e2 = 2.0L * f - f * f;    

    // Konwersja do radianów z wyższą precyzją
    long double lat1 = referenceLatitude * DEG_TO_RAD;
    long double lon1 = referenceLongitude * DEG_TO_RAD;
    long double lat2 = latitude * DEG_TO_RAD;
    long double lon2 = longitude * DEG_TO_RAD;
    
    // Bardziej precyzyjne obliczenia
    long double N1 = a / sqrtl(1.0L - e2 * sinl(lat1) * sinl(lat1));
    long double N2 = a / sqrtl(1.0L - e2 * sinl(lat2) * sinl(lat2));
    
    // Obliczenia ECEF z wyższą precyzją
    long double x1 = (N1 + referenceAltitude) * cosl(lat1) * cosl(lon1);
    long double y1 = (N1 + referenceAltitude) * cosl(lat1) * sinl(lon1);
    long double z1 = (N1 * (1.0L - e2) + referenceAltitude) * sinl(lat1);
    
    long double x2 = (N2 + altitude) * cosl(lat2) * cosl(lon2);
    long double y2 = (N2 + altitude) * cosl(lat2) * sinl(lon2);
    long double z2 = (N2 * (1.0L - e2) + altitude) * sinl(lat2);
    
    // Precyzyjne różnice współrzędnych
    long double dx = x2 - x1;
    long double dy = y2 - y1;
    long double dz = z2 - z1;
    
    // Transformacja z wyższą precyzją
    long double east  = -sinl(lon1) * dx + cosl(lon1) * dy;
    long double north = -sinl(lat1) * cosl(lon1) * dx 
                        - sinl(lat1) * sinl(lon1) * dy 
                        + cosl(lat1) * dz;
    long double up    = cosl(lat1) * cosl(lon1) * dx 
                        + cosl(lat1) * sinl(lon1) * dy 
                        + sinl(lat1) * dz;
    
    // Konwersja z powrotem do double
    return Vector3D(static_cast<double>(east), 
                    static_cast<double>(north), 
                    static_cast<double>(up));
}

void GPSData::fromENU(const Vector3D& enuPosition, double referenceLatitude, double referenceLongitude, double referenceAltitude) {
    // Constants for coordinate conversion
    const double DEG_TO_RAD = M_PI / 180.0;
    const double RAD_TO_DEG = 180.0 / M_PI;
    
    // WGS84 ellipsoid parameters
    const double a = 6378137.0;          // Semi-major axis length [m]
    const double f = 1.0 / 298.257223563; // Flattening
    const double e2 = 2.0 * f - f * f;    // First eccentricity squared
    
    // Convert reference point to radians
    double refLat = referenceLatitude * DEG_TO_RAD;
    double refLon = referenceLongitude * DEG_TO_RAD;
    
    // Extract ENU coordinates
    double east = enuPosition.getX();
    double north = enuPosition.getY();
    double up = enuPosition.getZ();
    
    // Calculate prime vertical radius of curvature for reference point
    double N_ref = a / sqrt(1.0 - e2 * sin(refLat) * sin(refLat));
    
    // ECEF coordinates of reference point
    double x_ref = (N_ref + referenceAltitude) * cos(refLat) * cos(refLon);
    double y_ref = (N_ref + referenceAltitude) * cos(refLat) * sin(refLon);
    double z_ref = (N_ref * (1.0 - e2) + referenceAltitude) * sin(refLat);
    
    // ENU to ECEF transformation matrix
    double dX = -sin(refLon) * east - sin(refLat) * cos(refLon) * north + cos(refLat) * cos(refLon) * up;
    double dY = cos(refLon) * east - sin(refLat) * sin(refLon) * north + cos(refLat) * sin(refLon) * up;
    double dZ = cos(refLat) * north + sin(refLat) * up;
    
    // Calculate ECEF coordinates of the target point
    double x = x_ref + dX;
    double y = y_ref + dY;
    double z = z_ref + dZ;
    
    // Convert ECEF to geodetic coordinates
    double p = sqrt(x*x + y*y);
    
    // Iterative calculation of latitude
    double lat = atan2(z, p * (1.0 - e2));
    for (int i = 0; i < 5; ++i) {
        double N = a / sqrt(1.0 - e2 * sin(lat) * sin(lat));
        double h = p / cos(lat) - N;
        double latNew = atan2(z, p * (1.0 - e2 * N / (N + h)));
        
        if (fabs(lat - latNew) < 1e-9) break;
        lat = latNew;
    }
    
    // Calculate longitude and altitude
    double lon = atan2(y, x);
    double N = a / sqrt(1.0 - e2 * sin(lat) * sin(lat));
    double h = p / cos(lat) - N;
    
    // Convert back to degrees
    latitude = lat * RAD_TO_DEG;
    longitude = lon * RAD_TO_DEG;
    altitude = h;
}

double GPSData::distanceTo(const GPSData& other) const {
    // Constant for converting degrees to radians
    const double DEG_TO_RAD = M_PI / 180.0;
    
    // WGS84 ellipsoid parameters
    const double a = 6378137.0;         // Semi-major axis [m]
    const double f = 1.0 / 298.257223563; // Flattening
    const double e2 = 2.0 * f - f * f;    // First eccentricity squared
    
    // Convert to radians
    double lat1 = latitude * DEG_TO_RAD;
    double lon1 = longitude * DEG_TO_RAD;
    double lat2 = other.latitude * DEG_TO_RAD;
    double lon2 = other.longitude * DEG_TO_RAD;
    
    // Curvature radius in the prime vertical for both points
    double N1 = a / sqrt(1.0 - e2 * sin(lat1) * sin(lat1));
    double N2 = a / sqrt(1.0 - e2 * sin(lat2) * sin(lat2));
    
    // ECEF Cartesian coordinates of point 1
    double X1 = (N1 + altitude) * cos(lat1) * cos(lon1);
    double Y1 = (N1 + altitude) * cos(lat1) * sin(lon1);
    double Z1 = (N1 * (1.0 - e2) + altitude) * sin(lat1);
    
    // ECEF Cartesian coordinates of point 2
    double X2 = (N2 + other.altitude) * cos(lat2) * cos(lon2);
    double Y2 = (N2 + other.altitude) * cos(lat2) * sin(lon2);
    double Z2 = (N2 * (1.0 - e2) + other.altitude) * sin(lat2);
    
    // Calculate distance
    double dX = X2 - X1;
    double dY = Y2 - Y1;
    double dZ = Z2 - Z1;
    
    // Return distance
    return sqrt(dX*dX + dY*dY + dZ*dZ);
}