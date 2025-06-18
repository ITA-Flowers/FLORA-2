// SensorData.cpp
#include "SensorData.hpp"
#include <algorithm>

SensorData::SensorData()
    : timestamp(0.0)
    , gpsData()
    , imuData()
    , hasGPS(false)
    , hasIMU(false)
{
}

SensorData::SensorData(double timestamp)
    : timestamp(timestamp)
    , gpsData()
    , imuData()
    , hasGPS(false)
    , hasIMU(false)
{
}

SensorData::SensorData(double timestamp, const GPSData& gpsData, const IMUData& imuData)
    : timestamp(timestamp)
    , gpsData(gpsData)
    , imuData(imuData)
    , hasGPS(true)
    , hasIMU(true)
{
}

bool SensorData::isValid() const {
    // Check if at least one type of data is present
    if (!hasGPS && !hasIMU) {
        return false;
    }
    
    // Check timestamp (must be >= 0)
    if (timestamp < 0.0) {
        return false;
    }
    
    // Validate GPS data if present
    if (hasGPS && !gpsData.isValid()) {
        return false;
    }
    
    // Validate IMU data if present
    if (hasIMU && !imuData.isValid()) {
        return false;
    }
    
    // All checks passed
    return true;
}

SensorData SensorData::interpolate(const SensorData& first, const SensorData& second, double targetTime) {
    // Check if targetTime is within range
    if (targetTime <= first.timestamp) {
        return first;
    }
    if (targetTime >= second.timestamp) {
        return second;
    }
    
    // Calculate interpolation factor
    double t = (targetTime - first.timestamp) / (second.timestamp - first.timestamp);
    
    // Create result with interpolated timestamp
    SensorData result(targetTime);
    
    // Interpolate GPS data if both records have it
    if (first.hasGPS && second.hasGPS) {
        GPSData interpolatedGPS;
        
        // Linear interpolation of GPS coordinates
        interpolatedGPS.setLatitude(first.gpsData.getLatitude() * (1-t) + second.gpsData.getLatitude() * t);
        interpolatedGPS.setLongitude(first.gpsData.getLongitude() * (1-t) + second.gpsData.getLongitude() * t);
        interpolatedGPS.setAltitude(first.gpsData.getAltitude() * (1-t) + second.gpsData.getAltitude() * t);
        
        // Interpolate accuracy (larger value = worse accuracy)
        interpolatedGPS.setAccuracy(std::max(first.gpsData.getAccuracy(), second.gpsData.getAccuracy()));
        
        // Use the more conservative fix type
        interpolatedGPS.setFixType(std::min(first.gpsData.getFixType(), second.gpsData.getFixType()));
        
        // Use minimum of satellite counts
        interpolatedGPS.setSatelliteCount(std::min(first.gpsData.getSatelliteCount(), second.gpsData.getSatelliteCount()));
        
        result.setGPSData(interpolatedGPS);
    } else if (first.hasGPS) {
        result.setGPSData(first.gpsData);
    } else if (second.hasGPS) {
        result.setGPSData(second.gpsData);
    }
    
    // Interpolate IMU data if both records have it
    if (first.hasIMU && second.hasIMU) {
        // Linear interpolation of accelerometer data
        Vector3D interpolatedAccel(
            first.imuData.getAccelerometer().getX() * (1-t) + second.imuData.getAccelerometer().getX() * t,
            first.imuData.getAccelerometer().getY() * (1-t) + second.imuData.getAccelerometer().getY() * t,
            first.imuData.getAccelerometer().getZ() * (1-t) + second.imuData.getAccelerometer().getZ() * t
        );
        
        // Linear interpolation of gyroscope data
        Vector3D interpolatedGyro(
            first.imuData.getGyroscope().getX() * (1-t) + second.imuData.getGyroscope().getX() * t,
            first.imuData.getGyroscope().getY() * (1-t) + second.imuData.getGyroscope().getY() * t,
            first.imuData.getGyroscope().getZ() * (1-t) + second.imuData.getGyroscope().getZ() * t
        );
        
        // Linear interpolation of magnetometer data
        Vector3D interpolatedMag(
            first.imuData.getMagnetometer().getX() * (1-t) + second.imuData.getMagnetometer().getX() * t,
            first.imuData.getMagnetometer().getY() * (1-t) + second.imuData.getMagnetometer().getY() * t,
            first.imuData.getMagnetometer().getZ() * (1-t) + second.imuData.getMagnetometer().getZ() * t
        );
        
        // Linear interpolation of temperature
        double interpolatedTemp = first.imuData.getTemperature() * (1-t) + second.imuData.getTemperature() * t;
        
        // Create interpolated IMU data
        IMUData interpolatedIMU(interpolatedAccel, interpolatedGyro, interpolatedMag, interpolatedTemp);
        
        result.setIMUData(interpolatedIMU);
    } else if (first.hasIMU) {
        result.setIMUData(first.imuData);
    } else if (second.hasIMU) {
        result.setIMUData(second.imuData);
    }
    
    return result;
}