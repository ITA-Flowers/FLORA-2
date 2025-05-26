// SensorData.hpp
#pragma once

#include "GPSData.hpp"
#include "IMUData.hpp"

/**
 * @brief Class for aggregating and synchronizing sensor data
 * 
 * This class holds timestamp and data from various sensors (GPS, IMU)
 * and provides methods for data validation and interpolation.
 */
class SensorData {
public:
    /**
     * @brief Default constructor
     */
    SensorData();
    
    /**
     * @brief Constructor with timestamp
     * 
     * @param timestamp Time in seconds since start
     */
    explicit SensorData(double timestamp);
    
    /**
     * @brief Full constructor
     * 
     * @param timestamp Time in seconds since start
     * @param gpsData GPS data
     * @param imuData IMU data
     */
    SensorData(double timestamp, const GPSData& gpsData, const IMUData& imuData);
    
    // Getters
    double getTimestamp() const { return timestamp; }
    GPSData getGPSData() const { return gpsData; }
    IMUData getIMUData() const { return imuData; }
    
    // Setters
    void setTimestamp(double ts) { timestamp = ts; }
    void setGPSData(const GPSData& gps) { gpsData = gps; hasGPS = true; }
    void setIMUData(const IMUData& imu) { imuData = imu; hasIMU = true; }
    
    /**
     * @brief Checks if GPS data is available
     * 
     * @return true if GPS data is present
     */
    bool hasGPSData() const { return hasGPS; }
    
    /**
     * @brief Checks if IMU data is available
     * 
     * @return true if IMU data is present
     */
    bool hasIMUData() const { return hasIMU; }
    
    /**
     * @brief Validates all sensor data
     * 
     * @return true if all present data is valid
     */
    bool isValid() const;
    
    /**
     * @brief Linearly interpolates between two SensorData points
     * 
     * @param other Other SensorData point
     * @param targetTime Target timestamp
     * @return Interpolated SensorData
     */
    static SensorData interpolate(const SensorData& first, const SensorData& second, double targetTime);

private:
    double timestamp;   // Time in seconds since start
    GPSData gpsData;    // GPS data
    IMUData imuData;    // IMU data
    bool hasGPS;        // Indicates if GPS data is present
    bool hasIMU;        // Indicates if IMU data is present
};