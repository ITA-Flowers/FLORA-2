#include "farneback_gpu.hpp"
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>

float computeFarnebackGpuMagnitude(const cv::Mat& prevGray, const cv::Mat& currGray) {
    static auto farneback = cv::cuda::FarnebackOpticalFlow::create();

    cv::cuda::GpuMat gpuPrev, gpuCurr, gpuFlow;
    gpuPrev.upload(prevGray);
    gpuCurr.upload(currGray);

    farneback->calc(gpuPrev, gpuCurr, gpuFlow);

    std::vector<cv::cuda::GpuMat> flowChannels(2);
    cv::cuda::split(gpuFlow, flowChannels);

    cv::cuda::GpuMat mag;
    cv::cuda::magnitude(flowChannels[0], flowChannels[1], mag);

    cv::Scalar avg = cv::cuda::sum(mag) / (mag.cols * mag.rows);
    return static_cast<float>(avg[0]);
}
