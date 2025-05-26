// IMUData.hpp
#pragma once

#include "core/Vector3D.hpp"

/**
 * @brief Class for storing and managing IMU sensor data
 * 
 * Stores accelerometer, gyroscope and magnetometer readings
 * with methods for accessing and manipulating the data.
 */
class IMUData {
public:
    /**
     * @brief Default constructor
     */
    IMUData();
    
    /**
     * @brief Constructor with sensor values
     * 
     * @param accelerometer Accelerometer readings (m/s²)
     * @param gyroscope Gyroscope readings (rad/s)
     * @param magnetometer Magnetometer readings (normalized)
     * @param temperature Sensor temperature (°C)
     */
    IMUData(const Vector3D& accelerometer, 
            const Vector3D& gyroscope, 
            const Vector3D& magnetometer,
            double temperature = 0.0);
    
    // Getters
    Vector3D getAccelerometer() const { return accelerometer; }
    Vector3D getGyroscope() const { return gyroscope; }
    Vector3D getMagnetometer() const { return magnetometer; }
    double getTemperature() const { return temperature; }
    
    // Setters
    void setAccelerometer(const Vector3D& accel) { accelerometer = accel; }
    void setGyroscope(const Vector3D& gyro) { gyroscope = gyro; }
    void setMagnetometer(const Vector3D& mag) { magnetometer = mag; }
    void setTemperature(double temp) { temperature = temp; }
    
    /**
     * @brief Normalizes the magnetometer readings
     */
    void normalizeMagnetometer();
    
    /**
     * @brief Validates IMU data for reasonable ranges
     * 
     * @return true if data is within acceptable ranges
     */
    bool isValid() const;

private:
    Vector3D accelerometer;  // Accelerometer readings in m/s²
    Vector3D gyroscope;      // Gyroscope readings in rad/s 
    Vector3D magnetometer;   // Magnetometer readings (normalized)
    double temperature;      // Sensor temperature in °C
};