#include <cmath>
#include <iostream>
#include <string>
#include "utils.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float calculateMetricScale(float droneAltitude, float cameraFovDeg, int imageHeight) {
    float fovRad = cameraFovDeg * static_cast<float>(M_PI) / 180.0f;
    return (2.0f * droneAltitude * std::tan(fovRad / 2.0f)) / imageHeight;
}
