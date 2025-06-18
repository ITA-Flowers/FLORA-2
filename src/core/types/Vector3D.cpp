// Vector3D.cpp
#include "Vector3D.hpp"
#include <cmath>
#include <stdexcept>

// Default constructor
Vector3D::Vector3D() : x(0.0), y(0.0), z(0.0) {}

// Parametric constructor
Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

// Getters
double Vector3D::getX() const {
    return x;
}

double Vector3D::getY() const {
    return y;
}

double Vector3D::getZ() const {
    return z;
}

// Setters
void Vector3D::setX(double x) {
    this->x = x;
}

void Vector3D::setY(double y) {
    this->y = y;
}

void Vector3D::setZ(double z) {
    this->z = z;
}

// Vector length (magnitude)
double Vector3D::magnitude() const {
    return std::sqrt(x*x + y*y + z*z);
}

// Normalized vector
Vector3D Vector3D::normalize() const {
    double mag = magnitude();
    if (mag < 1e-10) {  // arbitrary "close to zero" threshold
        throw std::domain_error("Cannot normalize vector of zero magnitude");
    }
    return Vector3D(x / mag, y / mag, z / mag);
}

// Scalar product (dot product)
double Vector3D::dot(const Vector3D& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Vector product (cross product)
Vector3D Vector3D::cross(const Vector3D& other) const {
    return Vector3D(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// Addition operator
Vector3D Vector3D::operator+(const Vector3D& other) const {
    return Vector3D(x + other.x, y + other.y, z + other.z);
}

// Subtraction operator
Vector3D Vector3D::operator-(const Vector3D& other) const {
    return Vector3D(x - other.x, y - other.y, z - other.z);
}

// Multiplication by scalar
Vector3D Vector3D::operator*(double scalar) const {
    return Vector3D(x * scalar, y * scalar, z * scalar);
}

// Division by scalar
Vector3D Vector3D::operator/(double scalar) const {
    if (std::abs(scalar) < 1e-10) {
        throw std::invalid_argument("Division by zero or near-zero");
    }
    return Vector3D(x / scalar, y / scalar, z / scalar);
}

// Comparison operator (equality)
bool Vector3D::operator==(const Vector3D& other) const {
    // Arbitrary small value for comparison
    const double epsilon = 1e-9;
    return (std::abs(x - other.x) < epsilon &&
            std::abs(y - other.y) < epsilon &&
            std::abs(z - other.z) < epsilon);
}

// Comparison operator (inequality)
bool Vector3D::operator!=(const Vector3D& other) const {
    return !(*this == other);
}

// Stream operator
std::ostream& operator<<(std::ostream& os, const Vector3D& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Static method: distance between two vectors
double Vector3D::distance(const Vector3D& v1, const Vector3D& v2) {
    return (v2 - v1).magnitude();
}

// Static method: linear interpolation between two vectors
Vector3D Vector3D::lerp(const Vector3D& v1, const Vector3D& v2, double t) {
    // Clamp t to [0, 1]
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    
    return v1 * (1.0 - t) + v2 * t;
}