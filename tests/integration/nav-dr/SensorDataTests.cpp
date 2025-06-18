// tests/unit/sensors/SensorDataTests.cpp
#include <gtest/gtest.h>
#include "nav-dr/sensors/SensorData.hpp"

class SensorDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup
    }
};

// Test constructors
TEST_F(SensorDataTest, Constructors) {
    // Default constructor
    SensorData data1;
    EXPECT_DOUBLE_EQ(data1.getTimestamp(), 0.0);
    EXPECT_FALSE(data1.hasGPSData());
    EXPECT_FALSE(data1.hasIMUData());
    
    // Timestamp constructor
    SensorData data2(10.5);
    EXPECT_DOUBLE_EQ(data2.getTimestamp(), 10.5);
    EXPECT_FALSE(data2.hasGPSData());
    EXPECT_FALSE(data2.hasIMUData());
    
    // Full constructor
    GPSData gps(37.7749, -122.4194, 10.0);
    IMUData imu(Vector3D(0.0, 9.8, 0.0), Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 1.0));
    
    SensorData data3(15.0, gps, imu);
    EXPECT_DOUBLE_EQ(data3.getTimestamp(), 15.0);
    EXPECT_TRUE(data3.hasGPSData());
    EXPECT_TRUE(data3.hasIMUData());
    EXPECT_DOUBLE_EQ(data3.getGPSData().getLatitude(), 37.7749);
    EXPECT_DOUBLE_EQ(data3.getIMUData().getAccelerometer().getY(), 9.8);
}

// Test getters and setters
TEST_F(SensorDataTest, GettersAndSetters) {
    SensorData data;
    
    // Test timestamp
    data.setTimestamp(25.5);
    EXPECT_DOUBLE_EQ(data.getTimestamp(), 25.5);
    
    // Test GPS data
    GPSData gps(40.7128, -74.0060, 15.5);
    data.setGPSData(gps);
    EXPECT_TRUE(data.hasGPSData());
    EXPECT_DOUBLE_EQ(data.getGPSData().getLatitude(), 40.7128);
    EXPECT_DOUBLE_EQ(data.getGPSData().getLongitude(), -74.0060);
    
    // Test IMU data
    IMUData imu(Vector3D(1.0, 2.0, 3.0), Vector3D(4.0, 5.0, 6.0), Vector3D(0.1, 0.2, 0.3));
    data.setIMUData(imu);
    EXPECT_TRUE(data.hasIMUData());
    EXPECT_DOUBLE_EQ(data.getIMUData().getAccelerometer().getX(), 1.0);
    EXPECT_DOUBLE_EQ(data.getIMUData().getGyroscope().getY(), 5.0);
}

// Test data validation
TEST_F(SensorDataTest, DataValidation) {
    // Valid sensor data with both GPS and IMU
    GPSData validGPS(37.7749, -122.4194, 10.0, 2.5, GPSData::FixType::FIX_3D, 8);
    IMUData validIMU(Vector3D(0.0, 9.8, 0.0), Vector3D(0.1, 0.2, 0.3), Vector3D(0.5, 0.5, 0.7071));
    
    SensorData validData(10.0, validGPS, validIMU);
    EXPECT_TRUE(validData.isValid());
    
    // Valid with only GPS
    SensorData validGPSOnly(10.0);
    validGPSOnly.setGPSData(validGPS);
    EXPECT_TRUE(validGPSOnly.isValid());
    
    // Valid with only IMU
    SensorData validIMUOnly(10.0);
    validIMUOnly.setIMUData(validIMU);
    EXPECT_TRUE(validIMUOnly.isValid());
    
    // Invalid timestamp
    SensorData invalidTimestamp(-1.0, validGPS, validIMU);
    EXPECT_FALSE(invalidTimestamp.isValid());
    
    // Invalid GPS data
    GPSData invalidGPS(91.0, -122.4194, 10.0); // Invalid latitude
    SensorData invalidGPSData(10.0, invalidGPS, validIMU);
    EXPECT_FALSE(invalidGPSData.isValid());
    
    // Invalid IMU data
    IMUData invalidIMU(Vector3D(0.0, 200.0, 0.0), Vector3D(0.1, 0.2, 0.3), Vector3D(0.5, 0.5, 0.7071)); // Accel too high
    SensorData invalidIMUData(10.0, validGPS, invalidIMU);
    EXPECT_FALSE(invalidIMUData.isValid());
    
    // No data at all
    SensorData emptyData(10.0);
    EXPECT_FALSE(emptyData.isValid());
}

// Test interpolation
TEST_F(SensorDataTest, Interpolation) {
    // Create two sensor data records at different timestamps
    GPSData gps1(37.7749, -122.4194, 10.0);
    GPSData gps2(37.7849, -122.4094, 20.0);
    
    IMUData imu1(Vector3D(0.0, 9.8, 0.0), Vector3D(0.1, 0.2, 0.3), Vector3D(0.1, 0.2, 0.3));
    IMUData imu2(Vector3D(1.0, 9.8, 0.0), Vector3D(0.4, 0.5, 0.6), Vector3D(0.4, 0.5, 0.6));
    
    SensorData data1(10.0, gps1, imu1);
    SensorData data2(20.0, gps2, imu2);
    
    // Interpolate at the halfway point (t=15.0)
    SensorData interpolated = SensorData::interpolate(data1, data2, 15.0);
    
    // Check timestamp
    EXPECT_DOUBLE_EQ(interpolated.getTimestamp(), 15.0);
    
    // Check GPS interpolation (should be halfway between the two points)
    EXPECT_DOUBLE_EQ(interpolated.getGPSData().getLatitude(), 37.7799);
    EXPECT_DOUBLE_EQ(interpolated.getGPSData().getLongitude(), -122.4144);
    EXPECT_DOUBLE_EQ(interpolated.getGPSData().getAltitude(), 15.0);
    
    // Check IMU interpolation
    EXPECT_DOUBLE_EQ(interpolated.getIMUData().getAccelerometer().getX(), 0.5);
    EXPECT_DOUBLE_EQ(interpolated.getIMUData().getGyroscope().getY(), 0.35);
    EXPECT_DOUBLE_EQ(interpolated.getIMUData().getMagnetometer().getZ(), 0.45);
    
    // Test interpolation at boundaries
    // At t=10.0, should return data1
    SensorData interp1 = SensorData::interpolate(data1, data2, 10.0);
    EXPECT_DOUBLE_EQ(interp1.getTimestamp(), 10.0);
    EXPECT_DOUBLE_EQ(interp1.getGPSData().getLatitude(), 37.7749);
    
    // At t=20.0, should return data2
    SensorData interp2 = SensorData::interpolate(data1, data2, 20.0);
    EXPECT_DOUBLE_EQ(interp2.getTimestamp(), 20.0);
    EXPECT_DOUBLE_EQ(interp2.getGPSData().getLatitude(), 37.7849);
    
    // Before t=10.0, should return data1
    SensorData interp3 = SensorData::interpolate(data1, data2, 5.0);
    EXPECT_DOUBLE_EQ(interp3.getTimestamp(), 10.0);
    EXPECT_DOUBLE_EQ(interp3.getGPSData().getLatitude(), 37.7749);
    
    // After t=20.0, should return data2
    SensorData interp4 = SensorData::interpolate(data1, data2, 25.0);
    EXPECT_DOUBLE_EQ(interp4.getTimestamp(), 20.0);
    EXPECT_DOUBLE_EQ(interp4.getGPSData().getLatitude(), 37.7849);
}

// Test partial data interpolation (only GPS or only IMU)
TEST_F(SensorDataTest, PartialDataInterpolation) {
    // Create sensor data with only GPS
    GPSData gps1(37.7749, -122.4194, 10.0);
    GPSData gps2(37.7849, -122.4094, 20.0);
    
    SensorData gpsOnly1(10.0);
    gpsOnly1.setGPSData(gps1);
    
    SensorData gpsOnly2(20.0);
    gpsOnly2.setGPSData(gps2);
    
    // Interpolate at t=15.0
    SensorData interpolated1 = SensorData::interpolate(gpsOnly1, gpsOnly2, 15.0);
    
    // Should have GPS data interpolated
    EXPECT_TRUE(interpolated1.hasGPSData());
    EXPECT_DOUBLE_EQ(interpolated1.getGPSData().getLatitude(), 37.7799);
    EXPECT_FALSE(interpolated1.hasIMUData());
    
    // Create sensor data with only IMU
    IMUData imu1(Vector3D(0.0, 9.8, 0.0), Vector3D(0.1, 0.2, 0.3), Vector3D(0.1, 0.2, 0.3));
    IMUData imu2(Vector3D(1.0, 9.8, 0.0), Vector3D(0.4, 0.5, 0.6), Vector3D(0.4, 0.5, 0.6));
    
    SensorData imuOnly1(10.0);
    imuOnly1.setIMUData(imu1);
    
    SensorData imuOnly2(20.0);
    imuOnly2.setIMUData(imu2);
    
    // Interpolate at t=15.0
    SensorData interpolated2 = SensorData::interpolate(imuOnly1, imuOnly2, 15.0);
    
    // Should have IMU data interpolated
    EXPECT_TRUE(interpolated2.hasIMUData());
    EXPECT_DOUBLE_EQ(interpolated2.getIMUData().getAccelerometer().getX(), 0.5);
    EXPECT_FALSE(interpolated2.hasGPSData());
    
    // Test interpolation between mixed data (one has GPS, one has IMU)
    SensorData mixed = SensorData::interpolate(gpsOnly1, imuOnly2, 15.0);
    
    // Should have both GPS and IMU from respective sources
    EXPECT_TRUE(mixed.hasGPSData());
    EXPECT_TRUE(mixed.hasIMUData());
    EXPECT_DOUBLE_EQ(mixed.getGPSData().getLatitude(), 37.7749);  // From gpsOnly1
    EXPECT_DOUBLE_EQ(mixed.getIMUData().getAccelerometer().getX(), 1.0);  // From imuOnly2
}