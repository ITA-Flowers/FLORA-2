#include <gtest/gtest.h>
#include "core/Vector3D.hpp"
#include <cmath>
#include <sstream>

class Vector3DTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Config before each test
    }

    void TearDown() override {
        // Clean up after each test
    }

    // Helper function for comparing double values
    bool isClose(double a, double b, double epsilon = 1e-9) {
        return std::abs(a - b) < epsilon;
    }
};

// Constructor tests
TEST_F(Vector3DTest, Constructors) {
    // Default constructor
    Vector3D v1;
    EXPECT_DOUBLE_EQ(v1.getX(), 0.0);
    EXPECT_DOUBLE_EQ(v1.getY(), 0.0);
    EXPECT_DOUBLE_EQ(v1.getZ(), 0.0);

    // Parametric constructor
    Vector3D v2(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v2.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v2.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v2.getZ(), 3.0);

    // Copy constructor
    Vector3D v3 = v2;
    EXPECT_DOUBLE_EQ(v3.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v3.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v3.getZ(), 3.0);
}

// Getters and setters tests
TEST_F(Vector3DTest, GettersAndSetters) {
    Vector3D v;
    
    // Setters test
    v.setX(5.0);
    v.setY(-2.0);
    v.setZ(7.5);
    
    // Getters test
    EXPECT_DOUBLE_EQ(v.getX(), 5.0);
    EXPECT_DOUBLE_EQ(v.getY(), -2.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 7.5);
}

// Calculate magnitude of a vector test
TEST_F(Vector3DTest, Magnitude) {
    Vector3D v1(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v1.magnitude(), 5.0);
    
    Vector3D v2(1.0, 1.0, 1.0);
    EXPECT_DOUBLE_EQ(v2.magnitude(), std::sqrt(3.0));
    
    Vector3D v3;
    EXPECT_DOUBLE_EQ(v3.magnitude(), 0.0);
}

// Normalize a vector test
TEST_F(Vector3DTest, Normalize) {
    Vector3D v1(3.0, 0.0, 0.0);
    Vector3D n1 = v1.normalize();
    EXPECT_DOUBLE_EQ(n1.getX(), 1.0);
    EXPECT_DOUBLE_EQ(n1.getY(), 0.0);
    EXPECT_DOUBLE_EQ(n1.getZ(), 0.0);
    EXPECT_DOUBLE_EQ(n1.magnitude(), 1.0);
    
    Vector3D v2(1.0, 1.0, 1.0);
    Vector3D n2 = v2.normalize();
    double expected = 1.0 / std::sqrt(3.0);
    EXPECT_DOUBLE_EQ(n2.getX(), expected);
    EXPECT_DOUBLE_EQ(n2.getY(), expected);
    EXPECT_DOUBLE_EQ(n2.getZ(), expected);
    EXPECT_NEAR(n2.magnitude(), 1.0, 1e-10);
    
    // Zero vector normalization should throw an exception
    Vector3D v3;
    EXPECT_THROW(v3.normalize(), std::domain_error);
}

// Test dot product
TEST_F(Vector3DTest, DotProduct) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(4.0, 5.0, 6.0);
    
    // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    EXPECT_DOUBLE_EQ(v1.dot(v2), 32.0);
    
    // Test commutativity
    EXPECT_DOUBLE_EQ(v1.dot(v2), v2.dot(v1));
    
    // Test for orthogonal vectors
    Vector3D v3(1.0, 0.0, 0.0);
    Vector3D v4(0.0, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(v3.dot(v4), 0.0);
}

// Test cross product
TEST_F(Vector3DTest, CrossProduct) {
    Vector3D v1(1.0, 0.0, 0.0);
    Vector3D v2(0.0, 1.0, 0.0);
    
    // i × j = k
    Vector3D cross1 = v1.cross(v2);
    EXPECT_DOUBLE_EQ(cross1.getX(), 0.0);
    EXPECT_DOUBLE_EQ(cross1.getY(), 0.0);
    EXPECT_DOUBLE_EQ(cross1.getZ(), 1.0);
    
    // j × i = -k
    Vector3D cross2 = v2.cross(v1);
    EXPECT_DOUBLE_EQ(cross2.getX(), 0.0);
    EXPECT_DOUBLE_EQ(cross2.getY(), 0.0);
    EXPECT_DOUBLE_EQ(cross2.getZ(), -1.0);
    
    // Test for arbitrary vectors
    Vector3D v3(2.0, 3.0, 4.0);
    Vector3D v4(5.0, 6.0, 7.0);
    Vector3D cross3 = v3.cross(v4);
    
    // (3*7 - 4*6, 4*5 - 2*7, 2*6 - 3*5)
    EXPECT_DOUBLE_EQ(cross3.getX(), 3.0*7.0 - 4.0*6.0);
    EXPECT_DOUBLE_EQ(cross3.getY(), 4.0*5.0 - 2.0*7.0);
    EXPECT_DOUBLE_EQ(cross3.getZ(), 2.0*6.0 - 3.0*5.0);
}

// Test addition operator
TEST_F(Vector3DTest, Addition) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(4.0, 5.0, 6.0);
    
    Vector3D sum = v1 + v2;
    EXPECT_DOUBLE_EQ(sum.getX(), 5.0);
    EXPECT_DOUBLE_EQ(sum.getY(), 7.0);
    EXPECT_DOUBLE_EQ(sum.getZ(), 9.0);
}

// Test subtraction operator
TEST_F(Vector3DTest, Subtraction) {
    Vector3D v1(5.0, 7.0, 9.0);
    Vector3D v2(1.0, 2.0, 3.0);
    
    Vector3D diff = v1 - v2;
    EXPECT_DOUBLE_EQ(diff.getX(), 4.0);
    EXPECT_DOUBLE_EQ(diff.getY(), 5.0);
    EXPECT_DOUBLE_EQ(diff.getZ(), 6.0);
}

// Test scalar multiplication
TEST_F(Vector3DTest, ScalarMultiplication) {
    Vector3D v(1.0, 2.0, 3.0);
    double scalar = 2.5;
    
    Vector3D result = v * scalar;
    EXPECT_DOUBLE_EQ(result.getX(), 2.5);
    EXPECT_DOUBLE_EQ(result.getY(), 5.0);
    EXPECT_DOUBLE_EQ(result.getZ(), 7.5);
}

// Test scalar division
TEST_F(Vector3DTest, ScalarDivision) {
    Vector3D v(2.5, 5.0, 7.5);
    double scalar = 2.5;
    
    Vector3D result = v / scalar;
    EXPECT_DOUBLE_EQ(result.getX(), 1.0);
    EXPECT_DOUBLE_EQ(result.getY(), 2.0);
    EXPECT_DOUBLE_EQ(result.getZ(), 3.0);
    
    // Division by zero should throw an exception
    EXPECT_THROW(v / 0.0, std::invalid_argument);
}

// Test comparison operators
TEST_F(Vector3DTest, Comparison) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(1.0, 2.0, 3.0);
    Vector3D v3(3.0, 2.0, 1.0);
    
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    
    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
    
    // Test for near-equality
    Vector3D v4(1.0, 2.0, 3.0);
    Vector3D v5(1.0 + 1e-10, 2.0, 3.0);
    EXPECT_TRUE(v4 == v5);
}

// Test stream operator
TEST_F(Vector3DTest, StreamOperator) {
    Vector3D v(1.5, -2.5, 3.0);
    
    std::ostringstream oss;
    oss << v;
    
    EXPECT_EQ(oss.str(), "(1.5, -2.5, 3)");
}

// Test magnitude of a vector
TEST_F(Vector3DTest, Distance) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(4.0, 6.0, 3.0);
    
    // sqrt((4-1)^2 + (6-2)^2 + (3-3)^2) = sqrt(9 + 16 + 0) = 5
    EXPECT_DOUBLE_EQ(Vector3D::distance(v1, v2), 5.0);
    
    // Test symmetry
    EXPECT_DOUBLE_EQ(Vector3D::distance(v1, v2), Vector3D::distance(v2, v1));
}

// Test linear interpolation
TEST_F(Vector3DTest, LinearInterpolation) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(5.0, 6.0, 7.0);
    
    // t = 0 should give v1
    Vector3D result1 = Vector3D::lerp(v1, v2, 0.0);
    EXPECT_TRUE(result1 == v1);
    
    // t = 1 should give v2
    Vector3D result2 = Vector3D::lerp(v1, v2, 1.0);
    EXPECT_TRUE(result2 == v2);
    
    // t = 0.5 should give average of v1 and v2
    Vector3D result3 = Vector3D::lerp(v1, v2, 0.5);
    EXPECT_DOUBLE_EQ(result3.getX(), 3.0);
    EXPECT_DOUBLE_EQ(result3.getY(), 4.0);
    EXPECT_DOUBLE_EQ(result3.getZ(), 5.0);
    
    // t < 0 or t > 1 should clamp to 0 or 1 respectively
    Vector3D result4 = Vector3D::lerp(v1, v2, -0.5);
    EXPECT_TRUE(result4 == v1);
    
    Vector3D result5 = Vector3D::lerp(v1, v2, 1.5);
    EXPECT_TRUE(result5 == v2);
}