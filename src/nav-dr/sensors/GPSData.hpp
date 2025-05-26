// GPSData.hpp
#pragma once

#include "core/Vector3D.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Class for storing and managing GPS data
 * 
 * Stores position in geographic coordinates (latitude, longitude, altitude)
 * and provides methods for coordinate conversions and data validation.
 */
class GPSData {
public:
    enum class FixType {
        NO_FIX = 0,
        FIX_2D = 2,
        FIX_3D = 3,
        RTK_FLOAT = 4,
        RTK_FIXED = 5
    };
    
    /**
     * @brief Default constructor
     */
    GPSData();
    
    /**
     * @brief Constructor with position data
     * 
     * @param latitude Latitude in degrees
     * @param longitude Longitude in degrees
     * @param altitude Altitude in meters
     * @param accuracy Position accuracy estimate in meters
     * @param fixType Type of GPS fix
     * @param satelliteCount Number of satellites used
     */
    GPSData(double latitude, double longitude, double altitude, 
            double accuracy = 0.0, 
            FixType fixType = FixType::FIX_3D,
            int satelliteCount = 0);
    
    // Getters
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    double getAltitude() const { return altitude; }
    double getAccuracy() const { return accuracy; }
    FixType getFixType() const { return fixType; }
    int getSatelliteCount() const { return satelliteCount; }
    
    // Setters
    void setLatitude(double lat) { latitude = lat; }
    void setLongitude(double lon) { longitude = lon; }
    void setAltitude(double alt) { altitude = alt; }
    void setAccuracy(double acc) { accuracy = acc; }
    void setFixType(FixType fix) { fixType = fix; }
    void setSatelliteCount(int count) { satelliteCount = count; }
    
    /**
     * @brief Checks if GPS data is valid
     * 
     * @return true if data is valid
     */
    bool isValid() const;
    
    /**
     * @brief Converts GPS coordinates to local ENU coordinates
     * 
     * @param referenceLatitude Reference latitude in degrees
     * @param referenceLongitude Reference longitude in degrees
     * @param referenceAltitude Reference altitude in meters
     * @return Position in ENU coordinates (East, North, Up) in meters
     */
    Vector3D toENU(double referenceLatitude, double referenceLongitude, double referenceAltitude) const;
    
    /**
     * @brief Converts local ENU coordinates to GPS coordinates
     * 
     * @param enuPosition Position in ENU coordinates (East, North, Up)
     * @param referenceLatitude Reference latitude in degrees
     * @param referenceLongitude Reference longitude in degrees
     * @param referenceAltitude Reference altitude in meters
     */
    void fromENU(const Vector3D& enuPosition, double referenceLatitude, double referenceLongitude, double referenceAltitude);
    
    /**
     * @brief Calculates distance between two GPS positions
     * 
     * @param other Another GPS position
     * @return Distance in meters
     */
    double distanceTo(const GPSData& other) const;

private:
    double latitude;       // Latitude in degrees
    double longitude;      // Longitude in degrees
    double altitude;       // Altitude in meters
    double accuracy;       // Position accuracy estimate in meters
    FixType fixType;       // Type of GPS fix
    int satelliteCount;    // Number of satellites used
};