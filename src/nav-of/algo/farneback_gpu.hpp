#pragma once
#include <opencv2/core.hpp>

float computeFarnebackGpuMagnitude(const cv::Mat& prevFrame, const cv::Mat& currFrame, int scaledHeight);

