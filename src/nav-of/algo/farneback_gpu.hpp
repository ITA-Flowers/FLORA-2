#pragma once
#include <opencv2/core.hpp>

float computeFarnebackGpuMagnitude(const cv::Mat& prevGray, const cv::Mat& currGray);
