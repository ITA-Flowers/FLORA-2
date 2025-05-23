#include "Config.hpp"
#include "../core/Vector3D.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

int ofProcess(Config config, OpticalFlowProcessor& ofProcessor) {
    std::cout << "  [*] Processing Optical Flow..." << std::endl;
    std::cout << "      - opening video file: " << config.getInputVideoFile() << std::endl;
    cv::VideoCapture cap(config.getInputVideoFile());
    if (!cap.isOpened()) {
        std::cerr << "[Error]: Could not open video file: " << config.getInputVideoFile() << std::endl;
        return 2;
    }

    std::string outputPath = "../../data/" + config.getOutputFile();
    std::cout << "      - opening output file: " << outputPath << std::endl;
    std::ofstream outFile(outputPath);

    if (!outFile.is_open()) {
        std::cerr << "[Error]: Could not open output file: " << outputPath << std::endl;
        return 3;
    }
    outFile << "frame_number,speed_mps\n";

    cv::Mat frame;
    int frameCount = 0;

    std::cout << "      - processing video frames:" << std::endl;
    while (cap.read(frame)) {
        if (ofProcessor.update(frame, config.getAltitudeM())) {
            Vector3D v = ofProcessor.getVelocity();
            std::cout << "         - frame: " << frameCount << "\tspeed: " << v.getX() << " m/s\r" << std::flush;
            outFile << frameCount << "," << v.getX() << "\n";
        }
        frameCount++;
    }

    std::cout << std::endl << "      - finished processing video frames." << std::endl;
    outFile.close();

    return 0;
}

int mainProcess(Config config) {
    std::cout << "\n Initializing:" << std::endl;

    std::cout << "  [*] Initializing Dead Reckoning Processor : ";
    std::cout << "Not implemented yet. - skip" << std::endl;

    std::cout << "  [*] Initializing Optical Flow Processor : ";
    OpticalFlowProcessor ofProcessor;
    ofProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
    ofProcessor.setFrameRate(config.getVideoFps());
    std::cout << "OK" << std::endl << std::endl;

    if (config.isOnlyOF()) {
        std::cout << "  - Processing only Optical Flow...\n" << std::endl;
        return ofProcess(config, ofProcessor);

    } else if (config.isOnlyDR()) {
        std::cout << "  - Processing only Dead Reckoning...\n" << std::endl;
        std::cerr << "  [!] Dead Reckoning is not implemented yet.\n" << std::endl;
        return 1;
    } else {
        std::cout << "  - Processing both Optical Flow and Dead Reckoning...\n" << std::endl;
        std::cerr << "  [!] Dead Reckoning is not implemented yet.\n" << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    int ret = 0;
    Config config = Config::parseCommandLine(argc, argv);

    if (config.isShowHelp()) {
        config.printHelp(argv[0]);
    } else if (config.isShowVersion()) {
        config.printVersion();
    } else {
        config.printSummary(config);
        ret = mainProcess(config);
    }

    std::cerr << std::endl << "[DEBUG] Final return code: " << ret << std::endl;
    if (ret != 0) {
        std::cerr << "Error: Exit code: " << ret << std::endl;
    }
    return ret;
}