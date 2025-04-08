#include "horn_schunck.hpp"

void hornSchunck(const cv::Mat& prev, const cv::Mat& curr, cv::Mat& u, cv::Mat& v, float alpha, int iterations) {
    cv::Mat I1, I2;
    prev.convertTo(I1, CV_32F);
    curr.convertTo(I2, CV_32F);

    cv::Mat Ix, Iy, It;
    cv::Sobel(I1, Ix, CV_32F, 1, 0, 3);
    cv::Sobel(I1, Iy, CV_32F, 0, 1, 3);
    It = I2 - I1;

    u = cv::Mat::zeros(I1.size(), CV_32F);
    v = cv::Mat::zeros(I1.size(), CV_32F);

    cv::Mat uAvg, vAvg, kernel = (cv::Mat_<float>(3,3) <<
        1/12.0, 1/6.0, 1/12.0,
        1/6.0,  0,    1/6.0,
        1/12.0, 1/6.0, 1/12.0
    );

    for (int i = 0; i < iterations; ++i) {
        cv::filter2D(u, uAvg, -1, kernel);
        cv::filter2D(v, vAvg, -1, kernel);

        cv::Mat numerator = Ix.mul(uAvg) + Iy.mul(vAvg) + It;
        cv::Mat denominator = alpha * alpha + Ix.mul(Ix) + Iy.mul(Iy);

        u = uAvg - Ix.mul(numerator) / denominator;
        v = vAvg - Iy.mul(numerator) / denominator;
    }
}
