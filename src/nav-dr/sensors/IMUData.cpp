// IMUData.cpp
#include "IMUData.hpp"
#include <cmath>

IMUData::IMUData()
    : accelerometer(Vector3D(0.0, 0.0, 0.0))
    , gyroscope(Vector3D(0.0, 0.0, 0.0))
    , magnetometer(Vector3D(0.0, 0.0, 0.0))
    , temperature(0.0)
{
}

IMUData::IMUData(const Vector3D& accelerometer, 
                 const Vector3D& gyroscope, 
                 const Vector3D& magnetometer,
                 double temperature)
    : accelerometer(accelerometer)
    , gyroscope(gyroscope)
    , magnetometer(magnetometer)
    , temperature(temperature)
{
}

void IMUData::normalizeMagnetometer() {
    double magnitude = magnetometer.magnitude();
    if (magnitude > 1e-10) {  // Avoid division by very small numbers
        magnetometer = magnetometer / magnitude;
    }
}

bool IMUData::isValid() const {
    // Check accelerometer data (typical ranges for consumer IMUs)
    // Max acceleration around ±16g (156.96 m/s²)
    const double MAX_ACCEL = 157.0;
    if (accelerometer.getX() < -MAX_ACCEL || accelerometer.getX() > MAX_ACCEL ||
        accelerometer.getY() < -MAX_ACCEL || accelerometer.getY() > MAX_ACCEL ||
        accelerometer.getZ() < -MAX_ACCEL || accelerometer.getZ() > MAX_ACCEL) {
        return false;
    }
    
    // Check gyroscope data (typical ranges around ±2000 deg/s = ±34.9 rad/s)
    const double MAX_GYRO = 35.0;
    if (gyroscope.getX() < -MAX_GYRO || gyroscope.getX() > MAX_GYRO ||
        gyroscope.getY() < -MAX_GYRO || gyroscope.getY() > MAX_GYRO ||
        gyroscope.getZ() < -MAX_GYRO || gyroscope.getZ() > MAX_GYRO) {
        return false;
    }
    
    // Check magnetometer data (should be normalized or close to Earth's field)
    const double MAX_MAG = 1.5; // For normalized data, slightly above 1.0
    if (magnetometer.getX() < -MAX_MAG || magnetometer.getX() > MAX_MAG ||
        magnetometer.getY() < -MAX_MAG || magnetometer.getY() > MAX_MAG ||
        magnetometer.getZ() < -MAX_MAG || magnetometer.getZ() > MAX_MAG) {
        return false;
    }
    
    // Check temperature (reasonable range for electronics)
    if (temperature < -40.0 || temperature > 125.0) {
        return false;
    }
    
    return true;
}