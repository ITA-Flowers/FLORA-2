// tests/core/QuaternionTests.cpp
#include <gtest/gtest.h>
#include "core/Quaternion.hpp"
#include "core/Vector3D.hpp"
#include <cmath>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class QuaternionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Config before each test
    }

    void TearDown() override {
        // Clean up after each test
    }

    // Helper method to compare quaternions with a small tolerance
    bool areQuaternionsEqual(const Quaternion& q1, const Quaternion& q2, double epsilon = 1e-9) {
        return (std::abs(q1.getW() - q2.getW()) < epsilon &&
                std::abs(q1.getX() - q2.getX()) < epsilon &&
                std::abs(q1.getY() - q2.getY()) < epsilon &&
                std::abs(q1.getZ() - q2.getZ()) < epsilon);
    }
};

// Test constructors
TEST_F(QuaternionTest, Constructors) {
    // Default constructor should create a unit quaternion (1, 0, 0, 0)
    Quaternion q1;
    EXPECT_DOUBLE_EQ(q1.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q1.getX(), 0.0);
    EXPECT_DOUBLE_EQ(q1.getY(), 0.0);
    EXPECT_DOUBLE_EQ(q1.getZ(), 0.0);

    // Constructor with 4 scalar parameters
    Quaternion q2(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(q2.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q2.getX(), 2.0);
    EXPECT_DOUBLE_EQ(q2.getY(), 3.0);
    EXPECT_DOUBLE_EQ(q2.getZ(), 4.0);

    // Constructor with scalar and vector parameters
    Vector3D v(2.0, 3.0, 4.0);
    Quaternion q3(1.0, v);
    EXPECT_DOUBLE_EQ(q3.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q3.getX(), 2.0);
    EXPECT_DOUBLE_EQ(q3.getY(), 3.0);
    EXPECT_DOUBLE_EQ(q3.getZ(), 4.0);

    // Copy constructor
    Quaternion q4 = q3;
    EXPECT_DOUBLE_EQ(q4.getW(), 1.0);
    EXPECT_DOUBLE_EQ(q4.getX(), 2.0);
    EXPECT_DOUBLE_EQ(q4.getY(), 3.0);
    EXPECT_DOUBLE_EQ(q4.getZ(), 4.0);
}

// Test getters and setters
TEST_F(QuaternionTest, GettersAndSetters) {
    Quaternion q;
    
    // Test setters
    q.setW(5.0);
    q.setX(-2.0);
    q.setY(7.5);
    q.setZ(3.25);
    
    // Test getters
    EXPECT_DOUBLE_EQ(q.getW(), 5.0);
    EXPECT_DOUBLE_EQ(q.getX(), -2.0);
    EXPECT_DOUBLE_EQ(q.getY(), 7.5);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.25);
    
    // Test direct access to components
    EXPECT_DOUBLE_EQ(q.getW(), 5.0);
    EXPECT_DOUBLE_EQ(q.getX(), -2.0);
    EXPECT_DOUBLE_EQ(q.getY(), 7.5);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.25);
    
    // Test vector getter and setter
    Vector3D v(1.0, 2.0, 3.0);
    q.setVector(v);
    EXPECT_DOUBLE_EQ(q.getX(), 1.0);
    EXPECT_DOUBLE_EQ(q.getY(), 2.0);
    EXPECT_DOUBLE_EQ(q.getZ(), 3.0);
    
    Vector3D v2 = q.getVector();
    EXPECT_TRUE(v == v2);
}

// Test norm calculation
TEST_F(QuaternionTest, Norm) {
    // Unit quaternion
    Quaternion q1;
    EXPECT_DOUBLE_EQ(q1.norm(), 1.0);
    
    // Quaternion with known norm
    Quaternion q2(2.0, 3.0, 4.0, 5.0);
    // sqrt(2^2 + 3^2 + 4^2 + 5^2) = sqrt(54) = 7.3484692283495345
    EXPECT_DOUBLE_EQ(q2.norm(), std::sqrt(54.0));
    
    // Zero quaternion
    Quaternion q3(0.0, 0.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(q3.norm(), 0.0);
}

// Test normalization
TEST_F(QuaternionTest, Normalization) {
    // Already normalized quaternion
    Quaternion q1(1.0, 0.0, 0.0, 0.0);
    Quaternion q1n = q1.normalized();
    EXPECT_TRUE(areQuaternionsEqual(q1, q1n));
    
    // Non-unit quaternion
    Quaternion q2(2.0, 3.0, 4.0, 5.0);
    double norm = q2.norm();
    
    // Test normalized() method
    Quaternion q2n = q2.normalized();
    EXPECT_DOUBLE_EQ(q2n.getW(), q2.getW() / norm);
    EXPECT_DOUBLE_EQ(q2n.getX(), q2.getX() / norm);
    EXPECT_DOUBLE_EQ(q2n.getY(), q2.getY() / norm);
    EXPECT_DOUBLE_EQ(q2n.getZ(), q2.getZ() / norm);
    EXPECT_NEAR(q2n.norm(), 1.0, 1e-10);
    
    // Test normalize() method
    Quaternion q3 = q2;
    q3.normalize();
    EXPECT_DOUBLE_EQ(q3.getW(), q2.getW() / norm);
    EXPECT_DOUBLE_EQ(q3.getX(), q2.getX() / norm);
    EXPECT_DOUBLE_EQ(q3.getY(), q2.getY() / norm);
    EXPECT_DOUBLE_EQ(q3.getZ(), q2.getZ() / norm);
    EXPECT_NEAR(q3.norm(), 1.0, 1e-10);
    
    // Test normalization of zero quaternion
    Quaternion q4(0.0, 0.0, 0.0, 0.0);
    EXPECT_THROW(q4.normalized(), std::domain_error);
    EXPECT_THROW(q4.normalize(), std::domain_error);
}

// Test conjugate
TEST_F(QuaternionTest, Conjugate) {
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    Quaternion qc = q.conjugate();
    
    EXPECT_DOUBLE_EQ(qc.getW(), q.getW());
    EXPECT_DOUBLE_EQ(qc.getX(), -q.getX());
    EXPECT_DOUBLE_EQ(qc.getY(), -q.getY());
    EXPECT_DOUBLE_EQ(qc.getZ(), -q.getZ());
}

// Test quaternion inverse
TEST_F(QuaternionTest, Inverse) {
    // For a unit quaternion, inverse = conjugate
    Quaternion q1(1.0, 0.0, 0.0, 0.0);
    Quaternion q1i = q1.inverse();
    EXPECT_TRUE(areQuaternionsEqual(q1i, q1.conjugate()));
    
    // For a general quaternion
    Quaternion q2(2.0, 3.0, 4.0, 5.0);
    Quaternion q2i = q2.inverse();
    double norm_squared = q2.norm() * q2.norm();
    
    EXPECT_DOUBLE_EQ(q2i.getW(), q2.getW() / norm_squared);
    EXPECT_DOUBLE_EQ(q2i.getX(), -q2.getX() / norm_squared);
    EXPECT_DOUBLE_EQ(q2i.getY(), -q2.getY() / norm_squared);
    EXPECT_DOUBLE_EQ(q2i.getZ(), -q2.getZ() / norm_squared);
    
    // q * q^(-1) should be identity quaternion
    Quaternion result = q2 * q2i;
    EXPECT_NEAR(result.getW(), 1.0, 1e-9);
    EXPECT_NEAR(result.getX(), 0.0, 1e-9);
    EXPECT_NEAR(result.getY(), 0.0, 1e-9);
    EXPECT_NEAR(result.getZ(), 0.0, 1e-9);
    
    // Test inverse of zero quaternion
    Quaternion q3(0.0, 0.0, 0.0, 0.0);
    EXPECT_THROW(q3.inverse(), std::domain_error);
}

// Test quaternion addition
TEST_F(QuaternionTest, Addition) {
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion q2(5.0, 6.0, 7.0, 8.0);
    
    Quaternion sum = q1 + q2;
    EXPECT_DOUBLE_EQ(sum.getW(), 6.0);
    EXPECT_DOUBLE_EQ(sum.getX(), 8.0);
    EXPECT_DOUBLE_EQ(sum.getY(), 10.0);
    EXPECT_DOUBLE_EQ(sum.getZ(), 12.0);
}

// Test quaternion subtraction
TEST_F(QuaternionTest, Subtraction) {
    Quaternion q1(5.0, 6.0, 7.0, 8.0);
    Quaternion q2(1.0, 2.0, 3.0, 4.0);
    
    Quaternion diff = q1 - q2;
    EXPECT_DOUBLE_EQ(diff.getW(), 4.0);
    EXPECT_DOUBLE_EQ(diff.getX(), 4.0);
    EXPECT_DOUBLE_EQ(diff.getY(), 4.0);
    EXPECT_DOUBLE_EQ(diff.getZ(), 4.0);
}

// Test quaternion multiplication
TEST_F(QuaternionTest, Multiplication) {
    // Identity quaternion multiplication
    Quaternion identity(1.0, 0.0, 0.0, 0.0);
    Quaternion q1(2.0, 3.0, 4.0, 5.0);
    
    Quaternion result1 = identity * q1;
    EXPECT_TRUE(areQuaternionsEqual(result1, q1));
    
    // General quaternion multiplication
    Quaternion q2(5.0, 6.0, 7.0, 8.0);
    Quaternion result2 = q1 * q2;
    
    // Manual calculation of quaternion multiplication
    // (w1, x1, y1, z1) * (w2, x2, y2, z2) = 
    // (w1*w2 - x1*x2 - y1*y2 - z1*z2,
    //  w1*x2 + x1*w2 + y1*z2 - z1*y2,
    //  w1*y2 - x1*z2 + y1*w2 + z1*x2,
    //  w1*z2 + x1*y2 - y1*x2 + z1*w2)
    
    double w = 2.0*5.0 - 3.0*6.0 - 4.0*7.0 - 5.0*8.0;
    double x = 2.0*6.0 + 3.0*5.0 + 4.0*8.0 - 5.0*7.0;
    double y = 2.0*7.0 + 4.0*5.0 + 5.0*6.0 - 3.0*8.0;
    double z = 2.0*8.0 + 5.0*5.0 + 3.0*7.0 - 4.0*6.0;
    
    EXPECT_DOUBLE_EQ(result2.getW(), w);
    EXPECT_DOUBLE_EQ(result2.getX(), x);
    EXPECT_DOUBLE_EQ(result2.getY(), y);
    EXPECT_DOUBLE_EQ(result2.getZ(), z);
    
    // Test scalar multiplication
    Quaternion q3(1.0, 2.0, 3.0, 4.0);
    double scalar = 2.5;
    
    Quaternion result3 = q3 * scalar;
    EXPECT_DOUBLE_EQ(result3.getW(), 2.5);
    EXPECT_DOUBLE_EQ(result3.getX(), 5.0);
    EXPECT_DOUBLE_EQ(result3.getY(), 7.5);
    EXPECT_DOUBLE_EQ(result3.getZ(), 10.0);
}

// Test vector rotation
TEST_F(QuaternionTest, VectorRotation) {
    // Create a quaternion representing a 90-degree rotation around the Z axis
    double angle = M_PI / 2.0;  // 90 degrees
    Vector3D axis(0.0, 0.0, 1.0);
    Quaternion rotationQ = Quaternion::fromAxisAngle(axis, angle);
    
    // Rotate a vector pointing along the X axis
    Vector3D v(1.0, 0.0, 0.0);
    Vector3D rotated = rotationQ * v;
    
    // After 90-degree Z rotation, (1,0,0) should become approximately (0,1,0)
    EXPECT_NEAR(rotated.getX(), 0.0, 1e-10);
    EXPECT_NEAR(rotated.getY(), 1.0, 1e-10);
    EXPECT_NEAR(rotated.getZ(), 0.0, 1e-10);
    
    // Test rotation by the identity quaternion (no rotation)
    Quaternion identity;
    Vector3D v2(1.0, 2.0, 3.0);
    Vector3D notRotated = identity * v2;
    
    EXPECT_TRUE(v2 == notRotated);
}

// Test comparison operators
TEST_F(QuaternionTest, ComparisonOperators) {
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion q2(1.0, 2.0, 3.0, 4.0);
    Quaternion q3(4.0, 3.0, 2.0, 1.0);
    
    EXPECT_TRUE(q1 == q2);
    EXPECT_FALSE(q1 == q3);
    
    EXPECT_FALSE(q1 != q2);
    EXPECT_TRUE(q1 != q3);
    
    // Test comparison with small differences
    Quaternion q4(1.0 + 1e-10, 2.0, 3.0, 4.0);
    EXPECT_TRUE(q1 == q4);  // Should be considered equal with small epsilon
}

// Test stream operator
TEST_F(QuaternionTest, StreamOperator) {
    Quaternion q(1.5, -2.5, 3.0, -4.0);
    
    std::ostringstream oss;
    oss << q;
    
    EXPECT_EQ(oss.str(), "1.5 + -2.5i + 3j + -4k");
}

// Test conversion between Euler angles and quaternion
TEST_F(QuaternionTest, EulerAnglesConversion) {
    // Test conversion from Euler angles to quaternion
    double roll = M_PI / 4.0;    // 45 degrees
    double pitch = M_PI / 6.0;   // 30 degrees
    double yaw = M_PI / 3.0;     // 60 degrees
    
    Quaternion q = Quaternion::fromEulerAngles(roll, pitch, yaw);
    
    // Test conversion back to Euler angles
    Vector3D angles = q.toEulerAngles();
    
    EXPECT_NEAR(angles.getX(), roll, 1e-10);   // Roll
    EXPECT_NEAR(angles.getY(), pitch, 1e-10);  // Pitch
    EXPECT_NEAR(angles.getZ(), yaw, 1e-10);    // Yaw
    
    // Test special case: pitch = +/- 90 degrees (gimbal lock)
    double roll2 = M_PI / 4.0;           // 45 degrees
    double pitch2 = M_PI / 2.0;          // 90 degrees (north pole)
    double yaw2 = M_PI / 3.0;            // 60 degrees
    
    Quaternion q2 = Quaternion::fromEulerAngles(roll2, pitch2, yaw2);
    Vector3D angles2 = q2.toEulerAngles();
    
    EXPECT_NEAR(angles2.getY(), pitch2, 1e-7);  // Pitch should be preserved
}

// Test conversion between axis-angle and quaternion
TEST_F(QuaternionTest, AxisAngleConversion) {
    // Create a quaternion from axis-angle representation
    Vector3D axis(1.0, 1.0, 1.0);
    axis = axis.normalize();  // Normalize the axis
    double angle = M_PI / 3.0;  // 60 degrees
    
    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    
    // Check that the quaternion is properly formed
    double half_angle = angle / 2.0;
    double sin_half = std::sin(half_angle);
    
    EXPECT_NEAR(q.getW(), std::cos(half_angle), 1e-10);
    EXPECT_NEAR(q.getX(), axis.getX() * sin_half, 1e-10);
    EXPECT_NEAR(q.getY(), axis.getY() * sin_half, 1e-10);
    EXPECT_NEAR(q.getZ(), axis.getZ() * sin_half, 1e-10);
    
    // Test with zero axis (should throw)
    Vector3D zeroAxis(0.0, 0.0, 0.0);
    EXPECT_THROW(Quaternion::fromAxisAngle(zeroAxis, angle), std::invalid_argument);
}

// Test conversion between rotation matrix and quaternion
TEST_F(QuaternionTest, RotationMatrixConversion) {
    // Create a simple rotation matrix (90 degrees around Z)
    double matrix[3][3] = {
        {0.0, -1.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    
    Quaternion q = Quaternion::fromRotationMatrix(matrix);
    
    // For a 90-degree Z rotation, quaternion should be approximately (sqrt(2)/2, 0, 0, sqrt(2)/2)
    double expected = std::sqrt(2.0) / 2.0;
    EXPECT_NEAR(std::abs(q.getW()), expected, 1e-10);
    EXPECT_NEAR(std::abs(q.getZ()), expected, 1e-10);
    EXPECT_NEAR(q.getX(), 0.0, 1e-10);
    EXPECT_NEAR(q.getY(), 0.0, 1e-10);
}

// Test spherical interpolation (SLERP)
TEST_F(QuaternionTest, SphericalInterpolation) {
    // Create two quaternions (identity and 90-degree Z rotation)
    Quaternion q1(1.0, 0.0, 0.0, 0.0);
    Quaternion q2 = Quaternion::fromAxisAngle(Vector3D(0.0, 0.0, 1.0), M_PI / 2.0);
    
    // Test interpolation at t = 0
    Quaternion result1 = Quaternion::slerp(q1, q2, 0.0);
    EXPECT_TRUE(areQuaternionsEqual(result1, q1));
    
    // Test interpolation at t = 1
    Quaternion result2 = Quaternion::slerp(q1, q2, 1.0);
    EXPECT_TRUE(areQuaternionsEqual(result2, q2));
    
    // Test interpolation at t = 0.5
    Quaternion result3 = Quaternion::slerp(q1, q2, 0.5);
    
    // For a 45-degree Z rotation, quaternion should be approximately (sqrt(2+sqrt(2))/2, 0, 0, sqrt(2-sqrt(2))/2)
    Quaternion expected = Quaternion::fromAxisAngle(Vector3D(0.0, 0.0, 1.0), M_PI / 4.0);
    EXPECT_TRUE(areQuaternionsEqual(result3, expected, 1e-10));
    
    // Test out-of-range t values
    Quaternion result4 = Quaternion::slerp(q1, q2, -0.5);
    EXPECT_TRUE(areQuaternionsEqual(result4, q1));
    
    Quaternion result5 = Quaternion::slerp(q1, q2, 1.5);
    EXPECT_TRUE(areQuaternionsEqual(result5, q2));
}