// Quaternion.cpp
#include "Quaternion.hpp"
#include <cmath>
#include <stdexcept>

// default constructor (identity quaternion)
Quaternion::Quaternion() : w(1.0), v(Vector3D()) {}

// parametric constructor
Quaternion::Quaternion(double w, double x, double y, double z)
    : w(w), v(Vector3D(x, y, z)) {}

// parametric constructor with vector part
Quaternion::Quaternion(double w, const Vector3D& v)
    : w(w), v(v) {}

// Getters
double Quaternion::getW() const {
    return w;
}

double Quaternion::getX() const {
    return v.getX();
}

double Quaternion::getY() const {
    return v.getY();
}

double Quaternion::getZ() const {
    return v.getZ();
}

Vector3D Quaternion::getVector() const {
    return v;
}

// Setters
void Quaternion::setW(double w) {
    this->w = w;
}

void Quaternion::setX(double x) {
    this->v.setX(x);
}

void Quaternion::setY(double y) {
    this->v.setY(y);
}

void Quaternion::setZ(double z) {
    this->v.setZ(z);
}

void Quaternion::setVector(const Vector3D& v) {
    this->v = v;
}

// Quaternion norm
double Quaternion::norm() const {
    return std::sqrt(w*w + v.getX()*v.getX() + v.getY()*v.getY() + v.getZ()*v.getZ());
}

// Quaternion normalization
void Quaternion::normalize() {
    double n = norm();
    if (n < 1e-10) {
        throw std::domain_error("Cannot normalize quaternion with near-zero norm");
    }
    w /= n;
    v.setX(getX() / n);
    v.setY(getY() / n);
    v.setZ(getZ() / n);
}

// Normalized quaternion
Quaternion Quaternion::normalized() const {
    double n = norm();
    if (n < 1e-10) {
        throw std::domain_error("Cannot normalize quaternion with near-zero norm");
    }
    return Quaternion(w/n, v.getX()/n, v.getY()/n, v.getZ()/n);
}

// Quaternion conjugate
Quaternion Quaternion::conjugate() const {
    return Quaternion(w, -(v.getX()), -(v.getY()), -(v.getZ()));
}

// Quaternion inverse
Quaternion Quaternion::inverse() const {
    double n_squared = w*w + v.getX()*v.getX() + v.getY()*v.getY() + v.getZ()*v.getZ();
    if (n_squared < 1e-10) {
        throw std::domain_error("Cannot compute inverse of quaternion with near-zero norm");
    }
    return Quaternion(w/n_squared, -(v.getX())/n_squared, -(v.getY())/n_squared, -(v.getZ())/n_squared);
}

// Addition of quaternions
Quaternion Quaternion::operator+(const Quaternion& other) const {
    return Quaternion(this->w + other.getW(), this->getX() + other.getX(), this->getY() + other.getY(), this->getZ() + other.getZ());
}

// Subtraction of quaternions
Quaternion Quaternion::operator-(const Quaternion& other) const {
    return Quaternion(w - other.getW(), getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ());
}

// Multiplication of quaternions
Quaternion Quaternion::operator*(const Quaternion& other) const {
    return Quaternion(
        w * other.getW() - getX() * other.getX() - getY() * other.getY() - getZ() * other.getZ(),
        w * other.getX() + getX() * other.getW() + getY() * other.getZ() - getZ() * other.getY(),
        w * other.getY() + getY() * other.getW() + getZ() * other.getX() - getX() * other.getZ(),
        w * other.getZ() + getZ() * other.getW() + getX() * other.getY() - getY() * other.getX()
    );
}

// Multiplication by scalar
Quaternion Quaternion::operator*(double scalar) const {
    return Quaternion(w * scalar, v.getX() * scalar, v.getY() * scalar, v.getZ() * scalar);
}

// Rotate vector by quaternion
Vector3D Quaternion::operator*(const Vector3D& v) const {
    // Convert vector to quaternion
    Quaternion qv(0.0, v);
    
    // q * v * q^(-1)
    Quaternion result = (*this) * qv * this->conjugate();
    
    return result.getVector();
}

// Comparison operators
bool Quaternion::operator==(const Quaternion& other) const {
    const double epsilon = 1e-9;
    return (std::abs(this->w - other.getW()) < epsilon &&
            std::abs(this->getX() - other.getX()) < epsilon &&
            std::abs(this->getY() - other.getY()) < epsilon &&
            std::abs(this->getZ() - other.getZ()) < epsilon);
}

bool Quaternion::operator!=(const Quaternion& other) const {
    return !(*this == other);
}

// Stream operator
std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
    os << q.w << " + " << q.getX() << "i + " << q.getY() << "j + " << q.getZ() << "k";
    return os;
}

// Euler angles conversion (roll, pitch, yaw)
Vector3D Quaternion::toEulerAngles() const {
    double roll, pitch, yaw;
    
    // pitch (rotation around X axis)
    double sinp = 2.0 * (w * getY() - getZ() * getX());
    if (std::abs(sinp) >= 1.0) {
        // pitch out of range
        double angle = M_PI / 2.0;
        pitch = std::copysign(angle, sinp);
    } else {
        pitch = std::asin(sinp);
    }
    
    // yaw (rotation around Z axis)
    double siny_cosp = 2.0 * (w * getZ() + getX() * getY());
    double cosy_cosp = 1.0 - 2.0 * (getY() * getY() + getZ() * getZ());
    yaw = std::atan2(siny_cosp, cosy_cosp);
    
    // roll (rotation around Y axis)
    double sinr_cosp = 2.0 * (w * getX() + getY() * getZ());
    double cosr_cosp = 1.0 - 2.0 * (getX() * getX() + getY() * getY());
    roll = std::atan2(sinr_cosp, cosr_cosp);
    
    return Vector3D(roll, pitch, yaw);
}

// Quaternion from Euler angles
Quaternion Quaternion::fromEulerAngles(double roll, double pitch, double yaw) {
    // Convert angles to radians
    double cy = std::cos(yaw * 0.5);
    double sy = std::sin(yaw * 0.5);
    double cp = std::cos(pitch * 0.5);
    double sp = std::sin(pitch * 0.5);
    double cr = std::cos(roll * 0.5);
    double sr = std::sin(roll * 0.5);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.setX(sr * cp * cy - cr * sp * sy);
    q.setY(cr * sp * cy + sr * cp * sy);
    q.setZ(cr * cp * sy - sr * sp * cy);
    
    return q;
}

// Quaternion from axis-angle representation
Quaternion Quaternion::fromAxisAngle(const Vector3D& axis, double angle) {
    Vector3D normalized_axis;
    try {
        normalized_axis = axis.normalize();
    } catch (const std::domain_error&) {
        throw std::invalid_argument("Rotation axis cannot be a zero vector");
    }
    
    double half_angle = angle * 0.5;
    double sin_half = std::sin(half_angle);
    
    Quaternion q;
    q.w = std::cos(half_angle);
    q.setX(normalized_axis.getX() * sin_half);
    q.setY(normalized_axis.getY() * sin_half);
    q.setZ(normalized_axis.getZ() * sin_half);
    
    return q;
}

// Quaternion from rotation matrix
Quaternion Quaternion::fromRotationMatrix(const double matrix[3][3]) {
    double trace = matrix[0][0] + matrix[1][1] + matrix[2][2];
    Quaternion q;
    
    if (trace > 0.0) {
        double s = 0.5 / std::sqrt(trace + 1.0);
        q.w = 0.25 / s;
        q.setX((matrix[2][1] - matrix[1][2]) * s);
        q.setY((matrix[0][2] - matrix[2][0]) * s);
        q.setZ((matrix[1][0] - matrix[0][1]) * s);
    } else {
        if (matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2]) {
            double s = 2.0 * std::sqrt(1.0 + matrix[0][0] - matrix[1][1] - matrix[2][2]);
            q.w = (matrix[2][1] - matrix[1][2]) / s;
            q.setX(0.25 * s);
            q.setY((matrix[0][1] + matrix[1][0]) / s);
            q.setZ((matrix[0][2] + matrix[2][0]) / s);
        } else if (matrix[1][1] > matrix[2][2]) {
            double s = 2.0 * std::sqrt(1.0 + matrix[1][1] - matrix[0][0] - matrix[2][2]);
            q.w = (matrix[0][2] - matrix[2][0]) / s;
            q.setX((matrix[0][1] + matrix[1][0]) / s);
            q.setY(0.25 * s);
            q.setZ((matrix[1][2] + matrix[2][1]) / s);
        } else {
            double s = 2.0 * std::sqrt(1.0 + matrix[2][2] - matrix[0][0] - matrix[1][1]);
            q.w = (matrix[1][0] - matrix[0][1]) / s;
            q.setX((matrix[0][2] + matrix[2][0]) / s);
            q.setY((matrix[1][2] + matrix[2][1]) / s);
            q.setZ(0.25 * s);
        }
    }
    
    return q.normalized();
}

// Spherical linear interpolation between two quaternions
Quaternion Quaternion::slerp(const Quaternion& q1, const Quaternion& q2, double t) {
    // t in range [0, 1]
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    
    // Normalize input quaternions
    Quaternion v0 = q1.normalized();
    Quaternion v1 = q2.normalized();
    
    // Dot product
    double dot = v0.getW() * v1.getW() + v0.getX() * v1.getX() + v0.getY() * v1.getY() + v0.getZ() * v1.getZ();
    
    // If the quaternions are close, use linear interpolation
    if (std::abs(dot) > 0.9995) {
        Quaternion result = v0 * (1.0 - t) + v1 * t;
        return result.normalized();
    }
    
    // Clamp dot product to the range [-1, 1]
    if (dot < 0.0) {
        v1 = v1 * -1.0;
        dot = -dot;
    }
    
    // Angle between quaternions
    if (dot > 1.0) dot = 1.0;
    if (dot < -1.0) dot = -1.0;
    
    double theta_0 = std::acos(dot);
    double theta = theta_0 * t;
    
    // v2 = v1 - v0 * dot
    Quaternion v2 = v1 - v0 * dot;
    v2.normalize();
    
    // Quaternion result
    return v0 * std::cos(theta) + v2 * std::sin(theta);
}