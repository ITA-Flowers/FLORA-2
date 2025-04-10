#include "utils.hpp"
#include <cmath>

float calculateMetricScale(float droneAltitude, float cameraFovDeg, int imageHeight) {
    constexpr float PI = 3.14159265358979323846f;
    float fovRad = cameraFovDeg * PI / 180.0f;
    return (2.0f * droneAltitude * std::tan(fovRad / 2.0f)) / imageHeight;
}
