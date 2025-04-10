#pragma once
#include <opencv2/core.hpp>

void hornSchunck(const cv::Mat& prevGray, const cv::Mat& currGray,
                 cv::Mat& u, cv::Mat& v, float alpha = 1.0f, int iterations = 100);
