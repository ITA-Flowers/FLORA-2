#pragma once
#include <opencv2/opencv.hpp>
#include "kalman_filter.hpp"

class OpticalFlowRealtimeProcessor {
public:
    OpticalFlowRealtimeProcessor(float fps, float droneAltitude, float cameraFovDeg, int imageHeight);
    void pushFrame(const cv::Mat& frame);
    float getLastSpeed() const;

private:
    float fps_;
    float metricScale_;
    cv::Mat prevGray_;
    bool hasPrev_ = false;
    float lastSpeed_ = -1.0f;
    Kalman1D kalman_;
};
