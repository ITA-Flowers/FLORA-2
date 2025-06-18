// Vector3D.hpp
#pragma once

#include <cmath>
#include <iostream>

class Vector3D {
public:
    // ctors
    Vector3D();
    Vector3D(double x, double y, double z);
    
    // copy ctor, copy assignment
    Vector3D(const Vector3D& other) = default;
    Vector3D& operator=(const Vector3D& other) = default;
    
    // dtor
    ~Vector3D() = default;
    
    // Getters & Setters
    double getX() const;
    double getY() const;
    double getZ() const;
    void setX(double x);
    void setY(double y);
    void setZ(double z);
    
    // Vector operations
    double magnitude() const;
    Vector3D normalize() const;
    double dot(const Vector3D& other) const;
    Vector3D cross(const Vector3D& other) const;
    
    // Operators
    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(double scalar) const;
    Vector3D operator/(double scalar) const;
    
    // Comparison operators
    bool operator==(const Vector3D& other) const;
    bool operator!=(const Vector3D& other) const;
    
    // Stream operator
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec);
    
    // Static methods
    static double distance(const Vector3D& v1, const Vector3D& v2);
    static Vector3D lerp(const Vector3D& v1, const Vector3D& v2, double t);

private:
    // Data members
    double x;
    double y;
    double z;
};