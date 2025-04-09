#include "process_video.hpp"
#include "horn_schunck.hpp"
#include "kalman_filter.hpp"
#include "utils.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void processVideo(const std::string& videoPath,
                  float droneAltitude,
                  float cameraFovDeg,
                  int imageHeight) {
    fs::path path(videoPath);
    std::string outputCsv = path.stem().string() + "-output.csv";

    float metricScale = calculateMetricScale(droneAltitude, cameraFovDeg, imageHeight);

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Błąd: Nie można otworzyć pliku wideo: " << videoPath << std::endl;
        return;
    }

    Kalman1D kalman;

    cv::Mat prevFrame, prevGray;
    cap >> prevFrame;
    if (prevFrame.empty()) {
        std::cerr << "Błąd: Nie można odczytać pierwszej klatki z pliku: " << videoPath << std::endl;
        return;
    }
    cv::cvtColor(prevFrame, prevGray, cv::COLOR_BGR2GRAY);

    int frameCounter = 0;
    std::ofstream outFile(outputCsv);
    if (!outFile.is_open()) {
        std::cerr << "Błąd: Nie można otworzyć pliku do zapisu: " << outputCsv << std::endl;
        return;
    }

    outFile << "Frame,Speed_mps" << std::endl;

    while (true) {
        cv::Mat frame;
        for (int i = 0; i < 3 && cap.read(frame); ++i);

        if (frame.empty()) break;
        frameCounter += 3;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::Mat u, v;
        hornSchunck(prevGray, gray, u, v);

        cv::Mat mag;
        cv::magnitude(u, v, mag);
        double avgMag = cv::mean(mag)[0];

        float rawSpeed = avgMag * metricScale * cap.get(cv::CAP_PROP_FPS);
        float smoothedSpeed = kalman.update(rawSpeed);

        outFile << frameCounter << "," << smoothedSpeed << std::endl;
        std::cout << "Frame " << frameCounter << " | Speed: " << smoothedSpeed << " m/s" << std::endl;

        prevGray = gray.clone();
    }

    outFile.close();
    std::cout << "Zapisano wyniki do: " << outputCsv << std::endl;
}