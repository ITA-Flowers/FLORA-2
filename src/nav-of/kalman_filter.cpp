#include "kalman_filter.hpp"

Kalman1D::Kalman1D(float processNoise, float measurementNoise, float estimateError, float initialValue)
    : q(processNoise), r(measurementNoise), p(estimateError), x(initialValue), k(0) {}

float Kalman1D::update(float measurement) {
    p += q;
    k = p / (p + r);
    x += k * (measurement - x);
    p *= (1 - k);
    return x;
}
