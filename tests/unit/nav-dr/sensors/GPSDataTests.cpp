// tests/unit/sensors/GPSDataTests.cpp
#include <gtest/gtest.h>
#include "nav-dr/sensors/GPSData.hpp"
#include <cmath>

class GPSDataTest : public ::testing::Test {
protected:
    // Earth's equatorial radius in meters
    const double EARTH_RADIUS = 6371000.0;
    
    // Convert degrees to radians
    double deg2rad(double deg) { return deg * M_PI / 180.0; }
    
    // Convert radians to degrees
    double rad2deg(double rad) { return rad * 180.0 / M_PI; }
};

// Test constructors
TEST_F(GPSDataTest, Constructors) {
    // Default constructor
    GPSData gps1;
    EXPECT_DOUBLE_EQ(gps1.getLatitude(), 0.0);
    EXPECT_DOUBLE_EQ(gps1.getLongitude(), 0.0);
    EXPECT_DOUBLE_EQ(gps1.getAltitude(), 0.0);
    EXPECT_DOUBLE_EQ(gps1.getAccuracy(), 0.0);
    EXPECT_EQ(gps1.getFixType(), GPSData::FixType::NO_FIX);
    EXPECT_EQ(gps1.getSatelliteCount(), 0);
    
    // Parameterized constructor
    GPSData gps2(37.7749, -122.4194, 10.0, 2.5, GPSData::FixType::FIX_3D, 8);
    EXPECT_DOUBLE_EQ(gps2.getLatitude(), 37.7749);
    EXPECT_DOUBLE_EQ(gps2.getLongitude(), -122.4194);
    EXPECT_DOUBLE_EQ(gps2.getAltitude(), 10.0);
    EXPECT_DOUBLE_EQ(gps2.getAccuracy(), 2.5);
    EXPECT_EQ(gps2.getFixType(), GPSData::FixType::FIX_3D);
    EXPECT_EQ(gps2.getSatelliteCount(), 8);
}

// Test getters and setters
TEST_F(GPSDataTest, GettersAndSetters) {
    GPSData gps;
    
    // Test setters
    gps.setLatitude(40.7128);
    gps.setLongitude(-74.0060);
    gps.setAltitude(15.5);
    gps.setAccuracy(3.2);
    gps.setFixType(GPSData::FixType::RTK_FIXED);
    gps.setSatelliteCount(12);
    
    // Test getters
    EXPECT_DOUBLE_EQ(gps.getLatitude(), 40.7128);
    EXPECT_DOUBLE_EQ(gps.getLongitude(), -74.0060);
    EXPECT_DOUBLE_EQ(gps.getAltitude(), 15.5);
    EXPECT_DOUBLE_EQ(gps.getAccuracy(), 3.2);
    EXPECT_EQ(gps.getFixType(), GPSData::FixType::RTK_FIXED);
    EXPECT_EQ(gps.getSatelliteCount(), 12);
}

// Test validity
TEST_F(GPSDataTest, Validity) {
    // Valid GPS data
    GPSData validGPS(37.7749, -122.4194, 10.0, 2.5, GPSData::FixType::FIX_3D, 8);
    EXPECT_TRUE(validGPS.isValid());
    
    // Invalid latitude (out of range)
    GPSData invalidLatGPS(91.0, -122.4194, 10.0, 2.5, GPSData::FixType::FIX_3D, 8);
    EXPECT_FALSE(invalidLatGPS.isValid());
    
    // Invalid longitude (out of range)
    GPSData invalidLonGPS(37.7749, 181.0, 10.0, 2.5, GPSData::FixType::FIX_3D, 8);
    EXPECT_FALSE(invalidLonGPS.isValid());
    
    // Invalid altitude (too high)
    GPSData invalidAltGPS(37.7749, -122.4194, 10000.0, 2.5, GPSData::FixType::FIX_3D, 8);
    EXPECT_FALSE(invalidAltGPS.isValid());
    
    // Invalid fix type (NO_FIX)
    GPSData invalidFixGPS(37.7749, -122.4194, 10.0, 2.5, GPSData::FixType::NO_FIX, 8);
    EXPECT_FALSE(invalidFixGPS.isValid());
    
    // Invalid satellite count (too few)
    GPSData invalidSatGPS(37.7749, -122.4194, 10.0, 2.5, GPSData::FixType::FIX_3D, 2);
    EXPECT_FALSE(invalidSatGPS.isValid());
}

// Test ENU conversion
TEST_F(GPSDataTest, ENUConversion) {
    // Reference point (San Francisco)
    double refLat = 37.7749;
    double refLon = -122.4194;
    double refAlt = 0.0;
    
    // Test point about 1km East and 1km North from reference
    // This is approximately 0.009 degrees latitude and 0.012 degrees longitude at this location
    GPSData gps(37.7839, -122.4074, 10.0);  // ~1km North, ~1km East, 10m Up
    
    // Convert to ENU
    Vector3D enu = gps.toENU(refLat, refLon, refAlt);
    
    // Check ENU coordinates (approx 1000m East, 1000m North, 10m Up)
    EXPECT_NEAR(enu.getX(), 1000.0, 80.0);  // East, within 80m (due to Earth curvature and approximation)
    EXPECT_NEAR(enu.getY(), 1000.0, 80.0);  // North, within 80m
    EXPECT_NEAR(enu.getZ(), 10.0, 0.2);    // Up, within 0.2m
    
    // Convert back to GPS
    GPSData gps2;
    gps2.fromENU(enu, refLat, refLon, refAlt);
    
    // Check the values are close to original
    EXPECT_NEAR(gps2.getLatitude(), gps.getLatitude(), 1e-4);
    EXPECT_NEAR(gps2.getLongitude(), gps.getLongitude(), 1e-4);
    EXPECT_NEAR(gps2.getAltitude(), gps.getAltitude(), 1e-1);
}

// Test distance calculation
TEST_F(GPSDataTest, DistanceCalculation) {
    // Two GPS positions about 1km apart (San Francisco to ~1km NE)
    GPSData gps1(37.7749, -122.4194, 0.0);
    GPSData gps2(37.7839, -122.4074, 0.0);
    
    // Calculate distance
    double distance = gps1.distanceTo(gps2);
    
    // Should be roughly 1.4km (1km North + 1km East)
    EXPECT_NEAR(distance, 1414.0, 50.0);  // Within 50m of expected
    
    // Distance should be symmetric
    EXPECT_DOUBLE_EQ(gps1.distanceTo(gps2), gps2.distanceTo(gps1));
    
    // Test with altitude difference
    GPSData gps3(37.7749, -122.4194, 0.0);
    GPSData gps4(37.7749, -122.4194, 100.0);  // Same lat/lon, 100m higher
    
    // Distance should be 100m
    EXPECT_NEAR(gps3.distanceTo(gps4), 100.0, 0.1);
}