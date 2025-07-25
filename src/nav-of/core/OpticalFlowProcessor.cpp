#include "OpticalFlowProcessor.hpp"
#include "../algo/horn_schunck.hpp"
#include "../algo/farneback_gpu.hpp"
#include "../algo/utils.hpp"
#include <cmath>
#include <opencv2/imgproc.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

OpticalFlowProcessor::OpticalFlowProcessor() {}

void OpticalFlowProcessor::setCameraParams(double focalLength, const std::pair<int, int>& resolution) {
    focalLengthMm_ = static_cast<float>(focalLength);
    imageHeight_ = resolution.second;
}

void OpticalFlowProcessor::setFrameRate(float fps) {
    fps_ = fps;
}

float OpticalFlowProcessor::getFrameRate() const {
    return fps_;
}

bool OpticalFlowProcessor::update(const cv::Mat& frame, double altitude) {
    if (frame.empty() || focalLengthMm_ == 0.0f || imageHeight_ == 0 || fps_ <= 0.0f) return false;

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    if (!hasPrev_) {
        prevGray_ = gray.clone();
        hasPrev_ = true;
        return false;
    }

    // Zakładamy, że kamera ma poziomy FOV, a przeskalowujemy do 640x360
    int scaledWidth = 640;
    int scaledHeight = 360;
    int scaledDiagonal = static_cast<int>(std::sqrt(scaledWidth * scaledWidth + scaledHeight * scaledHeight));
    float metricScale = calculateMetricScale(altitude, focalLengthMm_, scaledDiagonal);

    float avgMag = computeFarnebackGpuMagnitude(prevGray_, gray, scaledHeight);

    float rawSpeed = avgMag * metricScale * fps_;
    float filteredSpeed = kalman_.update(rawSpeed);

    currentVelocity_ = Vector3D(filteredSpeed, 0.0, 0.0);
    confidence_ = 1.0;

    prevGray_ = gray.clone();
    return true;
}

Vector3D OpticalFlowProcessor::getVelocity() const {
    return currentVelocity_;
}

double OpticalFlowProcessor::getHeading() const {
    return 0.0; // do rozbudowy w przyszłości
}

double OpticalFlowProcessor::getConfidenceScore() const {
    return confidence_;
}