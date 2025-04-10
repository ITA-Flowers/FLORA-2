#include "horn_schunck.hpp"
#include <opencv2/imgproc.hpp>

void hornSchunck(const cv::Mat& prevGray, const cv::Mat& currGray,
                 cv::Mat& u, cv::Mat& v, float alpha, int iterations) {
    cv::Mat I1 = prevGray.clone(), I2 = currGray.clone();
    I1.convertTo(I1, CV_32F);
    I2.convertTo(I2, CV_32F);

    cv::Mat Ix, Iy, It;
    cv::Sobel(I1, Ix, CV_32F, 1, 0, 3);
    cv::Sobel(I1, Iy, CV_32F, 0, 1, 3);
    It = I2 - I1;

    u = cv::Mat::zeros(I1.size(), CV_32F);
    v = cv::Mat::zeros(I1.size(), CV_32F);

    cv::Mat kernel = (cv::Mat_<float>(3,3) << 1/12.0f, 1/6.0f, 1/12.0f,
                                              1/6.0f,  0.0f, 1/6.0f,
                                              1/12.0f, 1/6.0f, 1/12.0f);

    for (int iter = 0; iter < iterations; ++iter) {
        cv::Mat uAvg, vAvg;
        cv::filter2D(u, uAvg, -1, kernel);
        cv::filter2D(v, vAvg, -1, kernel);

        cv::Mat num = Ix.mul(uAvg) + Iy.mul(vAvg) + It;
        cv::Mat denom = alpha * alpha + Ix.mul(Ix) + Iy.mul(Iy);

        u = uAvg - Ix.mul(num) / denom;
        v = vAvg - Iy.mul(num) / denom;
    }
}