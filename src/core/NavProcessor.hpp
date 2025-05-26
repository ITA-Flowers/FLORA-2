#pragma once

#include "../nav-dr/core/DeadReckoningProcessor.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"

class NavProcessor {
public:
    NavProcessor() = default;

    void setCameraParams(int fovDeg, const std::pair<int, int>& resolution) {
        opticalFlowProcessor_.setCameraParams(fovDeg, resolution);
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