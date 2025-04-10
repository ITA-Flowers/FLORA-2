#pragma once
#include "IOFProcessor.hpp"
#include "../algo/kalman_filter.hpp"

class OpticalFlowProcessor : public IOFProcessor {
public:
    OpticalFlowProcessor();

    bool update(const cv::Mat& frame, double deltaTime, double altitude) override;

    Vector3D getVelocity() const override;
    double getHeading() const override;

    void setCameraParams(double focalLength, const std::pair<int, int>& resolution) override;
    void setFrameRate(float fps) override;

    double getConfidenceScore() const override;

private:
    float focalLengthMm_ = 0.0f;
    int imageHeight_ = 0;
    float fps_ = 30.0f;

    Vector3D currentVelocity_{0.0, 0.0, 0.0};
    double confidence_ = 0.0;

    cv::Mat prevGray_;
    bool hasPrev_ = false;

    Kalman1D kalman_;
};