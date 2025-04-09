#pragma once
#include <opencv2/opencv.hpp>

void hornSchunck(const cv::Mat& prev, const cv::Mat& curr, cv::Mat& u, cv::Mat& v, float alpha = 1.0f, int iterations = 100);
