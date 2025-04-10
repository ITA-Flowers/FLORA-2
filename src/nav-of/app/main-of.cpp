#include "../core/OpticalFlowProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    OpticalFlowProcessor processor;
    processor.setCameraParams(3.6, {1920, 1080});
    processor.setFrameRate(30.0f);

    double altitude = 28.0;
    double lastTime = static_cast<double>(cv::getTickCount());

    cv::VideoCapture cap("C:/Projekty/FLORA-2/data/Mar-9th-08-31AM-video-processed.mp4");
    if (!cap.isOpened()) return 1;

    cv::Mat frame;
    while (cap.read(frame)) {
        double now = static_cast<double>(cv::getTickCount());
        double dt = (now - lastTime) / cv::getTickFrequency();
        lastTime = now;

        if (processor.update(frame, dt, altitude)) {
            Vector3D v = processor.getVelocity();
            std::cout << "Speed: " << v.x << " m/s\n";
        }

        cv::imshow("Video", frame);
        if (cv::waitKey(1) == 27) break;
    }
    return 0;
}
