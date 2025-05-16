#include "../core/OpticalFlowProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " <video_path> <fps> <altitude_m> <fov_camera_deg> <video_width_px> <video_height_px>\n";
        std::cerr << "Example: ./nav_of_main flight.mp4 30 28.0 91.0 1920 1080\n";
        return 1;
    }

    std::string videoPath = argv[1];
    float fps = std::stof(argv[2]);
    float altitude = std::stof(argv[3]);
    double fov = std::stod(argv[4]);
    int width = std::stoi(argv[5]);
    int height = std::stoi(argv[6]);

    OpticalFlowProcessor processor;
    processor.setCameraParams(fov, {width, height});
    processor.setFrameRate(fps);

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file: " << videoPath << std::endl;
        return 2;
    }

    std::ofstream outFile("speed_log.csv");
    outFile << "frame_number,speed_mps\n";  

    cv::Mat frame;
    int frameCount = 0;

    double lastTime = static_cast<double>(cv::getTickCount());

    while (cap.read(frame)) {
        double now = static_cast<double>(cv::getTickCount());
        double deltaTime = (now - lastTime) / cv::getTickFrequency();
        lastTime = now;

        if (processor.update(frame, deltaTime, altitude)) {
            Vector3D v = processor.getVelocity();
            std::cout << "Frame: " << frameCount << "\tSpeed: " << v.getX() << " m/s" << std::endl;
            outFile << frameCount << "," << v.getX() << "\n";
        }
        frameCount++;
    }

    outFile.close();
    return 0;
}
