#include "farneback_gpu.hpp"
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/core/cuda.hpp>

float computeFarnebackGpuMagnitude(const cv::Mat& prevFrame, const cv::Mat& currFrame) {
    cv::Mat prevSmall, currSmall;
    cv::resize(prevFrame, prevSmall, cv::Size(640, 360));
    cv::resize(currFrame, currSmall, cv::Size(640, 360));

    cv::Mat prevGray, currGray;

    if (prevSmall.channels() == 3)
        cv::cvtColor(prevSmall, prevGray, cv::COLOR_BGR2GRAY);
    else
        prevGray = prevSmall.clone();

    if (currSmall.channels() == 3)
        cv::cvtColor(currSmall, currGray, cv::COLOR_BGR2GRAY);
    else
        currGray = currSmall.clone();

    cv::cuda::GpuMat d_prev(prevGray);
    cv::cuda::GpuMat d_curr(currGray);

    cv::Ptr<cv::cuda::FarnebackOpticalFlow> fb = cv::cuda::FarnebackOpticalFlow::create(
        5,    // numLevels
        0.5,  // pyrScale
        false,  // fastPyramids
        13,   // winSize
        3,    // numIters
        5,    // polyN
        1.1,  // polySigma
        0     // flags
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
