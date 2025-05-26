#include "Config.hpp"
#include "../core/types/Vector3D.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"
#include "../nav-dr/core/DeadReckoningProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>


int mainProcess(Config config) {
    std::cout << "\n Initializing:" << std::endl;

    std::cout << "  [*] Initializing Dead Reckoning Processor : ";
    std::cout << "Not implemented yet. - skip" << std::endl;

    std::cout << "  [*] Initializing Optical Flow Processor : ";
    OpticalFlowProcessor ofProcessor;
    ofProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
    ofProcessor.setFrameRate(config.getVideoFps());
    std::cout << "OK" << std::endl << std::endl;

    std::cout << "  [*] Initializing Dead Reckoning Processor : ";
    DeadReckoningProcessor drProcessor;
    std::cout << "OK" << std::endl << std::endl;

    std::cout << "  [*] Processing..." << std::endl;
    std::cout << "      - opening input file: " << outputPath << std::endl;
    std::ifstream inFile(config.getInputLogFile());
    if (!inFile.is_open()) {
        std::cerr << "[Error]: Could not open input log file: " << config.getInputLogFile() << std::endl;
        return 1;
    }

    std::cout << "      - opening video file: " << config.getInputVideoFile() << std::endl;
    cv::VideoCapture cap(config.getInputVideoFile());
    if (!cap.isOpened()) {
        std::cerr << "[Error]: Could not open video file: " << config.getInputVideoFile() << std::endl;
        return 2;
    }

    // std::string outputPath = "../../data/" + config.getOutputFile();
    std::string outputPath = config.getOutputFile();
    std::cout << "      - opening output file: " << outputPath << std::endl;
    std::ofstream outFile(outputPath);

    if (!outFile.is_open()) {
        std::cerr << "[Error]: Could not open output file: " << outputPath << std::endl;
        return 3;
    }
    std::cout << "      - writing header to output file." << std::endl;
    outFile << "frame_number,speed_mps,altitude,heading,dr_lat,dr_lon,gps_lat,gps_lon\n";

    cv::Mat frame;
    int frameCount = 0;

    std::cout << "      - processing video frames:" << std::endl;
    while (cap.read(frame) && inFile.eof() == false) {
        // Read the next line from the log file
        std::string line;


        // TODO: altitude and heading should be read from the log file
        if (ofProcessor.update(frame, config.getAltitudeM())) {
            Vector3D v = ofProcessor.getVelocity();
            std::cout << "        - frame: " << frameCount << "\tspeed: " << v.getX() << " m/s\r" << std::flush;
            outFile << frameCount << "," << v.getX() << "\n";
        }
        frameCount++;
    }

    std::cout << std::endl << "      - finished processing." << std::endl;
    outFile.close();

    return 0;
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