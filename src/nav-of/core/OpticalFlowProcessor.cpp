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

    // Zakładamy skalowanie do 360 px wysokości
    const int scaledHeight = 360;
    const float resizeRatio = static_cast<float>(scaledHeight) / static_cast<float>(imageHeight_);
    const float virtualFocalLength = focalLengthMm_ * resizeRatio;

    // Przeliczamy FOV dla przeskalowanego obrazu
    float adjustedFovDeg = 2.0f * std::atan((scaledHeight * 0.5f) / virtualFocalLength) * (180.0f / static_cast<float>(M_PI));
    int scaledWidth = static_cast<int>(frame.cols * resizeRatio);
    int diagonal = static_cast<int>(std::sqrt(scaledWidth * scaledWidth + scaledHeight * scaledHeight));
    float metricScale = calculateMetricScale(altitude, adjustedFovDeg, diagonal);

    // Wywołujemy funkcję z dodatkowym argumentem
    float avgMag = computeFarnebackGpuMagnitude(prevGray_, gray, scaledHeight);

    constexpr float calibrationFactor = 0.2f;
    float rawSpeed = avgMag * metricScale * fps_ * calibrationFactor;
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