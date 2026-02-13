#include "farneback_gpu.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
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

    cv::Mat flow;
    cv::calcOpticalFlowFarneback(
        prevGray,
        currGray,
        flow,
        0.5,
        5,
        13,
        3,
        5,
        1.1,
        0
    );

    std::vector<cv::Mat> flowChannels(2);
    cv::split(flow, flowChannels);

    cv::Mat magnitude;
    cv::magnitude(flowChannels[0], flowChannels[1], magnitude);

    cv::Scalar sumMag = cv::sum(magnitude);
    float averageMag = static_cast<float>(sumMag[0]) / (magnitude.rows * magnitude.cols);

    return averageMag;
}
