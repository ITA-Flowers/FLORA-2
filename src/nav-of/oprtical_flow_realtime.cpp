#include "optical_flow_realtime.hpp"
#include "horn_schunck.hpp"
#include "utils.hpp"

OpticalFlowRealtimeProcessor::OpticalFlowRealtimeProcessor(float fps, float droneAltitude, float cameraFovDeg, int imageHeight)
    : fps_(fps), kalman_() {
    metricScale_ = calculateMetricScale(droneAltitude, cameraFovDeg, imageHeight);
}

void OpticalFlowRealtimeProcessor::pushFrame(const cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    if (!hasPrev_) {
        prevGray_ = gray;
        hasPrev_ = true;
        lastSpeed_ = -1.0f;
        return;
    }

    cv::Mat u, v;
    hornSchunck(prevGray_, gray, u, v);

    cv::Mat mag;
    cv::magnitude(u, v, mag);
    double avgMag = cv::mean(mag)[0];

    float rawSpeed = avgMag * metricScale_ * fps_;
    lastSpeed_ = kalman_.update(rawSpeed);

    prevGray_ = gray.clone();
}

float OpticalFlowRealtimeProcessor::getLastSpeed() const {
    return lastSpeed_;
}
