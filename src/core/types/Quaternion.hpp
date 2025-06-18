// Quaternion.hpp
#pragma once

#include "Vector3D.hpp"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Quaternion {
public:
    // ctors
    Quaternion();
    Quaternion(double w, double x, double y, double z);
    Quaternion(double w, const Vector3D& v);
    
    // copy and assignment ctors
    Quaternion(const Quaternion& other) = default;
    Quaternion& operator=(const Quaternion& other) = default;
    
    // dtor
    ~Quaternion() = default;
    
    // Getters and setters
    double getW() const;
    double getX() const;
    double getY() const;
    double getZ() const;
    Vector3D getVector() const;
    
    void setW(double w);
    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setVector(const Vector3D& v);
    
    // Operations
    double norm() const;
    void normalize();
    Quaternion normalized() const;
    Quaternion conjugate() const;
    Quaternion inverse() const;
    
    // Operators
    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator-(const Quaternion& other) const;
    Quaternion operator*(const Quaternion& other) const;
    Quaternion operator*(double scalar) const;
    Vector3D operator*(const Vector3D& v) const;  // Rotate vector by quaternion
    
    // Comparison operators
    bool operator==(const Quaternion& other) const;
    bool operator!=(const Quaternion& other) const;
    
    // Stream operator
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& q);
    
    // Conversions
    Vector3D toEulerAngles() const;  // Kąty w radianach (roll, pitch, yaw)
    static Quaternion fromEulerAngles(double roll, double pitch, double yaw);
    static Quaternion fromAxisAngle(const Vector3D& axis, double angle);
    static Quaternion fromRotationMatrix(const double matrix[3][3]);
    
    // Spherical linear interpolation
    static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t);

private:
    // Quaternion components
    double w;   // scalar part
    Vector3D v; // vector part
};