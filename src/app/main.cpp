#include <iostream>
#include "Config.hpp"
#include "../core/NavProcessor.hpp"


// int mainProcess(Config config) {
//     std::cout << "\n Initializing:" << std::endl;

//     std::cout << "  [*] Initializing Dead Reckoning Processor : ";
//     std::cout << "Not implemented yet. - skip" << std::endl;

//     std::cout << "  [*] Initializing Optical Flow Processor : ";
//     OpticalFlowProcessor ofProcessor;
//     ofProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
//     ofProcessor.setFrameRate(config.getVideoFps());
//     std::cout << "OK" << std::endl << std::endl;

//     std::cout << "  [*] Initializing Dead Reckoning Processor : ";
//     DeadReckoningProcessor drProcessor;
//     std::cout << "OK" << std::endl << std::endl;

//     std::cout << "  [*] Processing..." << std::endl;
//     std::cout << "      - opening input file: " << config.getInputLogFile() << std::endl;
//     std::ifstream inFile(config.getInputLogFile());
//     if (!inFile.is_open()) {
//         std::cerr << "[Error]: Could not open input log file: " << config.getInputLogFile() << std::endl;
//         return 1;
//     }

//     std::cout << "      - opening video file: " << config.getInputVideoFile() << std::endl;
//     cv::VideoCapture cap(config.getInputVideoFile());
//     if (!cap.isOpened()) {
//         std::cerr << "[Error]: Could not open video file: " << config.getInputVideoFile() << std::endl;
//         return 2;
//     }

//     // std::string outputPath = "../../data/" + config.getOutputFile();
//     std::string outputPath = config.getOutputFile();
//     std::cout << "      - opening output file: " << outputPath << std::endl;
//     std::ofstream outFile(outputPath);

//     if (!outFile.is_open()) {
//         std::cerr << "[Error]: Could not open output file: " << outputPath << std::endl;
//         return 3;
//     }
//     std::cout << "      - writing header to output file." << std::endl;
//     outFile << "frame_number,speed_mps,altitude,heading,dr_lat,dr_lon,gps_lat,gps_lon\n";

//     cv::Mat frame;
//     int frameCount = 0;

//     std::cout << "      - processing video frames:" << std::endl;
//     while (cap.read(frame) && inFile.eof() == false) {
//         // Read the next line from the log file
//         std::string line;


//         // TODO: altitude and heading should be read from the log file
//         if (ofProcessor.update(frame, config.getAltitudeM())) {
//             Vector3D v = ofProcessor.getVelocity();
//             std::cout << "        - frame: " << frameCount << "\tspeed: " << v.getX() << " m/s\r" << std::flush;
//             outFile << frameCount << "," << v.getX() << "\n";
//         }
//         frameCount++;
//     }

//     std::cout << std::endl << "      - finished processing." << std::endl;
//     outFile.close();

//     return 0;
// }
int initNavProcessor(NavProcessor& navProcessor, const Config& config) {
    // Set camera parameters
    navProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
    navProcessor.setFrameRate(config.getVideoFps());

    // Initialize input files
    if (navProcessor.initInput(config.getInputDir()) != 0) {
        std::cerr << "Error: Could not initialize input files." << std::endl;
        return 1;
    }

    // Initialize output files
    if (navProcessor.initOutput(config.getOutputDir()) != 0) {
        std::cerr << "Error: Could not initialize output files." << std::endl;
        return 2;
    }

    return 0;
}

int doProcessing(NavProcessor& navProcessor, const Config& config) {

    return 0; // Placeholder for processing logic
}

int main(int argc, char* argv[]) {
    int ret = 0;
    Config config = Config::parseCommandLine(argc, argv);

    if (config.isShowHelp()) {
        config.printHelp(argv[0]);
    } else if (config.isShowVersion()) {
        config.printVersion();
    } else {
        // Print configuration summary
        config.printSummary(config);
        
        // ---------------------------------------------------------------------------------------------------
        // Initialize Navigation Processor
        std::cout << "\nInitializing:" << std::endl;
        std::cout << "  [*] Initializing Navigation Processor: ";

        // -- Create an instance of NavProcessor
        NavProcessor navProcessor;

        // -- Initialize NavProcessor with configuration
        ret = initNavProcessor(navProcessor, config);
        if (ret != 0) {
            std::cerr << "Error: Could not initialize NavProcessor." << std::endl;
            return ret;
        }
        std::cout << "OK" << std::endl;

        // ---------------------------------------------------------------------------------------------------
        // Process the navigation data
        std::cout << "  [*] Processing..." << std::endl;
        ret = doProcessing(navProcessor, config);
        if (ret != 0) {
            std::cerr << "Error: Processing failed with code " << ret << "." << std::endl;
            return ret;
        }
        std::cout << "  [*] Processing completed successfully." << std::endl;
    }

    std::cerr << std::endl << "[DEBUG] Final return code: " << ret << std::endl;
    if (ret != 0) {
        std::cerr << "Error: Exit code: " << ret << std::endl;
    }
    return ret;
}