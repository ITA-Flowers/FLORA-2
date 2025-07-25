#pragma once
#include <opencv2/opencv.hpp>
#include "../../core/types/Vector3D.hpp"

class IOFProcessor {
public:
    virtual ~IOFProcessor() = default;

    virtual Vector3D getVelocity() const = 0;
    virtual double getHeading() const = 0;

    virtual bool update(const cv::Mat& frame, double altitude) = 0;

    virtual void setCameraParams(double fov, const std::pair<int, int>& resolution) = 0;
    virtual void setFrameRate(float fps) = 0;

    virtual float getFrameRate() const = 0;

    virtual double getConfidenceScore() const = 0;
};