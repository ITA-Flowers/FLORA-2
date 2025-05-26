#pragma once

#include "../nav-dr/core/DeadReckoningProcessor.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"

class NavProcessor {
public:
    NavProcessor() = default;

    void setCameraParams(int fovDeg, const cv::Size& frameSize) {
        opticalFlowProcessor_.setCameraParams(fovDeg, frameSize);
    }

    void setFrameRate(int fps) {
        opticalFlowProcessor_.setFrameRate(fps);
    }

    OpticalFlowProcessor& getOpticalFlowProcessor() {
        return opticalFlowProcessor_;
    }

    DeadReckoningProcessor& getDeadReckoningProcessor() {
        return deadReckoningProcessor_;
    }


private:
    OpticalFlowProcessor opticalFlowProcessor_;
    DeadReckoningProcessor deadReckoningProcessor_;
};