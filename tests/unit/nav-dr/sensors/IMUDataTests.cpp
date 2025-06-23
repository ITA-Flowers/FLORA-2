// tests/unit/sensors/IMUDataTests.cpp
#include <gtest/gtest.h>
#include "nav-dr/sensors/IMUData.hpp"

class IMUDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for all IMU tests
    }
};

// Test constructors
TEST_F(IMUDataTest, Constructors) {
    // Default constructor
    IMUData imu1;
    EXPECT_DOUBLE_EQ(imu1.getAccelerometer().getX(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getAccelerometer().getY(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getAccelerometer().getZ(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getGyroscope().getX(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getGyroscope().getY(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getGyroscope().getZ(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getMagnetometer().getX(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getMagnetometer().getY(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getMagnetometer().getZ(), 0.0);
    EXPECT_DOUBLE_EQ(imu1.getTemperature(), 0.0);
    
    // Parameterized constructor
    Vector3D accel(1.0, 2.0, 3.0);
    Vector3D gyro(4.0, 5.0, 6.0);
    Vector3D mag(0.1, 0.2, 0.3);
    double temp = 25.5;
    
    IMUData imu2(accel, gyro, mag, temp);
    EXPECT_DOUBLE_EQ(imu2.getAccelerometer().getX(), 1.0);
    EXPECT_DOUBLE_EQ(imu2.getAccelerometer().getY(), 2.0);
    EXPECT_DOUBLE_EQ(imu2.getAccelerometer().getZ(), 3.0);
    EXPECT_DOUBLE_EQ(imu2.getGyroscope().getX(), 4.0);
    EXPECT_DOUBLE_EQ(imu2.getGyroscope().getY(), 5.0);
    EXPECT_DOUBLE_EQ(imu2.getGyroscope().getZ(), 6.0);
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getX(), 0.1);
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getY(), 0.2);
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getZ(), 0.3);
    EXPECT_DOUBLE_EQ(imu2.getTemperature(), 25.5);
}

// Test getters and setters
TEST_F(IMUDataTest, GettersAndSetters) {
    IMUData imu;
    
    // Test setters
    Vector3D accel(1.5, -2.5, 9.8);
    Vector3D gyro(0.1, 0.2, 0.3);
    Vector3D mag(0.5, 0.6, 0.7);
    double temp = 22.5;
    
    imu.setAccelerometer(accel);
    imu.setGyroscope(gyro);
    imu.setMagnetometer(mag);
    imu.setTemperature(temp);
    
    // Test getters
    EXPECT_DOUBLE_EQ(imu.getAccelerometer().getX(), 1.5);
    EXPECT_DOUBLE_EQ(imu.getAccelerometer().getY(), -2.5);
    EXPECT_DOUBLE_EQ(imu.getAccelerometer().getZ(), 9.8);
    EXPECT_DOUBLE_EQ(imu.getGyroscope().getX(), 0.1);
    EXPECT_DOUBLE_EQ(imu.getGyroscope().getY(), 0.2);
    EXPECT_DOUBLE_EQ(imu.getGyroscope().getZ(), 0.3);
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getX(), 0.5);
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getY(), 0.6);
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getZ(), 0.7);
    EXPECT_DOUBLE_EQ(imu.getTemperature(), 22.5);
}

// Test magnetometer normalization
TEST_F(IMUDataTest, MagnetometerNormalization) {
    IMUData imu;
    Vector3D mag(3.0, 4.0, 0.0); // 3-4-5 triangle, magnitude = 5
    imu.setMagnetometer(mag);
    
    imu.normalizeMagnetometer();
    
    // Verify the magnitude is now ~1.0
    double magnitude = std::sqrt(
        imu.getMagnetometer().getX() * imu.getMagnetometer().getX() +
        imu.getMagnetometer().getY() * imu.getMagnetometer().getY() +
        imu.getMagnetometer().getZ() * imu.getMagnetometer().getZ()
    );
    
    EXPECT_NEAR(magnitude, 1.0, 1e-10);
    
    // Check individual components (3/5 = 0.6, 4/5 = 0.8)
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getX(), 0.6);
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getY(), 0.8);
    EXPECT_DOUBLE_EQ(imu.getMagnetometer().getZ(), 0.0);
    
    // Test normalization of zero vector (shouldn't change)
    IMUData imu2;
    Vector3D zeroMag(0.0, 0.0, 0.0);
    imu2.setMagnetometer(zeroMag);
    
    imu2.normalizeMagnetometer();
    
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getX(), 0.0);
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getY(), 0.0);
    EXPECT_DOUBLE_EQ(imu2.getMagnetometer().getZ(), 0.0);
}

// Test data validation
TEST_F(IMUDataTest, DataValidation) {
    // Valid data
    IMUData validIMU(
        Vector3D(0.0, 9.8, 0.0),     // 1g acceleration in Y (normal gravity)
        Vector3D(0.1, 0.2, 0.3),     // Small rotation rates
        Vector3D(0.5, 0.5, 0.7071),  // Normalized mag field
        25.0                        // Normal temperature
    );
    
    EXPECT_TRUE(validIMU.isValid());
    
    // Invalid accelerometer (too high)
    IMUData invalidAccel(
        Vector3D(0.0, 200.0, 0.0),   // > 16g, too high
        Vector3D(0.1, 0.2, 0.3),
        Vector3D(0.5, 0.5, 0.7071),
        25.0
    );
    
    EXPECT_FALSE(invalidAccel.isValid());
    
    // Invalid gyroscope (too high)
    IMUData invalidGyro(
        Vector3D(0.0, 9.8, 0.0),
        Vector3D(0.1, 50.0, 0.3),    // > 35 rad/s, too high
        Vector3D(0.5, 0.5, 0.7071),
        25.0
    );
    
    EXPECT_FALSE(invalidGyro.isValid());
    
    // Invalid magnetometer (out of range)
    IMUData invalidMag(
        Vector3D(0.0, 9.8, 0.0),
        Vector3D(0.1, 0.2, 0.3),
        Vector3D(2.0, 0.0, 0.0),     // Magnitude > 1.5, too high
        25.0
    );
    
    EXPECT_FALSE(invalidMag.isValid());
    
    // Invalid temperature (too high)
    IMUData invalidTemp(
        Vector3D(0.0, 9.8, 0.0),
        Vector3D(0.1, 0.2, 0.3),
        Vector3D(0.5, 0.5, 0.7071),
        150.0                       // > 125°C, too high
    );
    
    EXPECT_FALSE(invalidTemp.isValid());
}