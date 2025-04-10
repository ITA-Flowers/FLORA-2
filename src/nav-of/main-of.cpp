#include "optical_flow_realtime.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    OpticalFlowRealtimeProcessor processor(
        30.0f,   // FPS
        28.0f,   // Dron height (meters)
        82.2f,   // Camera FOV (degrees)
        1080     // Image height (pixels)
    );

    cv::VideoCapture cap("data/Mar-9th-08-31AM-video-processed.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video source." << std::endl;
        return 1;
    }

    cv::Mat frame;
    while (cap.read(frame)) {
        processor.pushFrame(frame);
        float speed = processor.getLastSpeed();

        if (speed >= 0.0f) {
            std::cout << "Speed: " << speed << " m/s" << std::endl;
        }

        cv::imshow("Video", frame);
        if (cv::waitKey(1) == 27) break;  // ESC to quit
    }

    return 0;
}
