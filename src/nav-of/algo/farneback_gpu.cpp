#include "farneback_gpu.hpp"
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/core/cuda.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float computeFarnebackGpuMagnitude(const cv::Mat& prevFrame, const cv::Mat& currFrame, int scaledHeight) {
    int scaledWidth = static_cast<int>(prevFrame.cols * (scaledHeight / static_cast<float>(prevFrame.rows)));

    cv::Mat prevSmall, currSmall;
    cv::resize(prevFrame, prevSmall, cv::Size(scaledWidth, scaledHeight));
    cv::resize(currFrame, currSmall, cv::Size(scaledWidth, scaledHeight));

    cv::Mat prevGray, currGray;
    if (prevSmall.channels() == 3)
        cv::cvtColor(prevSmall, prevGray, cv::COLOR_BGR2GRAY);
    else
        prevGray = prevSmall;

    if (currSmall.channels() == 3)
        cv::cvtColor(currSmall, currGray, cv::COLOR_BGR2GRAY);
    else
        currGray = currSmall;

    cv::cuda::GpuMat d_prev(prevGray);
    cv::cuda::GpuMat d_curr(currGray);

    auto fb = cv::cuda::FarnebackOpticalFlow::create(
        5, 0.5, false, 13, 3, 5, 1.1, 0
    );

    cv::cuda::GpuMat flowGpu;
    fb->calc(d_prev, d_curr, flowGpu);

    std::vector<cv::cuda::GpuMat> flowChannels(2);
    cv::cuda::split(flowGpu, flowChannels);

    cv::cuda::GpuMat magnitudeGpu;
    cv::cuda::magnitude(flowChannels[0], flowChannels[1], magnitudeGpu);

    cv::Scalar sumMag = cv::cuda::sum(magnitudeGpu);
    float averageMag = static_cast<float>(sumMag[0]) / (magnitudeGpu.rows * magnitudeGpu.cols);

    return averageMag;
}