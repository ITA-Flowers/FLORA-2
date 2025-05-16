// main.cpp
#include "Config.hpp"
#include "../core/Vector3D.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>

int ofProcess(Config config, OpticalFlowProcessor& ofProcessor) {
    std::cout << "  [*] Processing Optical Flow..." << std::endl;
    std::cout << "      - opening video file: " << config.getInputVideoFile() << std::endl;
    cv::VideoCapture cap(config.getInputVideoFile());
    if (!cap.isOpened()) {
        std::cerr << "[Error]: Could not open video file: " << config.getInputVideoFile() << std::endl;
        return 2;
    }

    std::cout << "      - opening output file: " << config.getOutputFile() << std::endl;
    std::ofstream outFile(config.getOutputFile());

    if (!outFile.is_open()) {
        std::cerr << "[Error]: Could not open output file: " << config.getOutputFile() << std::endl;
        return 3;
    }
    // Write header to output file
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
    std::cout << " Initializing:" << std::endl;
    
    // Initialize result variables
    // Vector3D vel_of;
    // Vector3D vel_dr;
    // Vector3D vel_combined;
    // float heading_of = 0.0;
    // float heading_dr = 0.0;
    // float heading_combined = 0.0;

    // Initialize Processors
    // -- Dead Reckoning Processor
    std::cout << "  [*] Initializing Dead Reckoning Processor : ";
    // TODO: Implement Dead Reckoning Processor
    std:: cout << "Not implemented yet. - skip" << std::endl;

    // -- Optical Flow Processor
    std::cout << "  [*] Initializing Optical Flow Processor : ";
    OpticalFlowProcessor ofProcessor;
    ofProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
    ofProcessor.setFrameRate(config.getVideoFps());
    std::cout << "OK" << std::endl << std::endl;

    // Process 
    if (config.isOnlyOF()) {
        std::cout << "  - Processing only Optical Flow..." << std::endl;
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
    int ret = 0; // return code

    // Parse command line arguments
    Config config = Config::parseCommandLine(argc, argv);

    // Check for errors in command line arguments
    if (config.isShowHelp()) { // - show help if requested
        config.printHelp(argv[0]);
    } else if (config.isShowVersion()) { // - show version if requested
        config.printVersion();
    } else {
        // - print summary of the configuration
        config.printSummary(config);
            
        // * START OFPROGRAM LOGIC *
        // - process log file
        ret = mainProcess(config);
        // * END OF PROGRAM LOGIC *
    }

    std::cerr << std::endl << "[DEBUG] Final return code: " << ret << std::endl;

    // - return code
    if (ret != 0) {
        std::cerr << "Error: Exit code: " << ret << std::endl;
    } 
    return ret;
}