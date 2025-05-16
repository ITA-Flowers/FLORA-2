// Config.cpp
#include "Config.hpp"
#include <iostream>


void Config::printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  -f, --file FILE       input log file\n"
              << "  -c, --video FILE      input video file\n"
              << "  -o, --output FILE     store output into file\n\n"
              
              << "  -F, --fps FPS         video frames per second (default: 30)\n"
              << "  -V, --fov FOV         camera field of view in degrees (default: 91)\n"
              << "  -W, --width WIDTH     video width in pixels (default: 1920)\n"
              << "  -H, --height HEIGHT   video height in pixels (default: 1080)\n"
              << "  -A, --alt ALTITUDE    altitude in meters (default: 100)\n\n"

              << "  -O, --only-of         only process optical flow\n"
              << "  -D, --only-dr         only process dead reckoning\n\n"

              << "  -v, --version         show version\n"
              << "  -h, --help            show this information\n";
}

void Config::printVersion(void) {
    std::cout << APP_NAME << " | ver. " << VERSION << " | " << std::endl;
}

void Config::printSummary(const Config config) {
    std::cout << "Configuration:" << std::endl;
    
    std::cout << " Paths:" << std::endl;
    std::cout << "  Input  file Log:      " << config.inputLogFile << std::endl;
    std::cout << "  Input  file Video:    " << config.inputVideoFile << std::endl;
    std::cout << "  Output file:          " << (config.outputFile.empty() ? "None" : config.outputFile) << std::endl;

    std::cout << " Video parameters:" << std::endl;
    std::cout << "  FPS:                  " << config.videoFps << std::endl;
    std::cout << "  FOV camera[deg]:      " << config.videoFovCameraDeg << std::endl;
    std::cout << "  Width[px]:            " << config.videoWidthPx << std::endl;
    std::cout << "  Height[px]:           " << config.videoHeightPx << std::endl;
    std::cout << "  Altitude[m]:          " << config.altitudeM << std::endl;

    std::cout << " Flags:" << std::endl;
    std::cout << "  Mode:                 " << (config.onlyOF ? "Optical Flow" : (config.onlyDR ? "Dead Reckoning" : "Both")) << std::endl;
}

Config Config::parseCommandLine(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            config.showHelp = true;
            return config;
        } else if (arg == "-v" || arg == "--version") {
            config.showVersion = true;
            return config;
        } else if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc) {
                config.inputLogFile = argv[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-c" || arg == "--video") {
            if (i + 1 < argc) {
                config.inputVideoFile = argv[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                config.outputFile = argv[++i];
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-F" || arg == "--fps") {
            if (i + 1 < argc) {
                config.videoFps = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-V" || arg == "--fov") {
            if (i + 1 < argc) {
                config.videoFovCameraDeg = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-W" || arg == "--width") {
            if (i + 1 < argc) {
                config.videoWidthPx = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-H" || arg == "--height") {
            if (i + 1 < argc) {
                config.videoHeightPx = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-A" || arg == "--alt") {
            if (i + 1 < argc) {
                config.altitudeM = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Option " << arg << " requires an argument.\n";
                config.showHelp = true;
                return config;
            }
        } else if (arg == "-O" || arg == "--only-of") {
            config.onlyOF = true;
        } else if (arg == "-D" || arg == "--only-dr") {
            config.onlyDR = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            config.showHelp = true;
            return config;
        }
    }

    if ((!config.showHelp && !config.showVersion) && (config.inputLogFile.empty() || config.inputVideoFile.empty())) {
        std::cerr << "Error: Not all required input files provided.\n" << std::endl;
        config.showHelp = true;
    }

    return config;
}
