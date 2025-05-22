#include "utils.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float calculateMetricScale(float altitude, float fovDeg, int imageDiagonalPx) {
    float fovRad = fovDeg * static_cast<float>(M_PI) / 180.0f;
    float diagonalMeters = 2.0f * altitude * std::tan(fovRad / 2.0f);
    return diagonalMeters / static_cast<float>(imageDiagonalPx);
}
