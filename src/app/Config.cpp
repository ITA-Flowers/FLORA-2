// Config.cpp
#include "Config.hpp"
#include <iostream>


void Config::printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  -f, --file FILE       input log file\n"
              << "  -v, --video FILE      input video file\n"
              << "  -o, --output FILE     store output into file\n"
              << "  -V, --version         show version\n"
              << "  -h, --help            show this information\n";
}

void Config::printVersion(void) {
    std::cout << APP_NAME << " | ver. " << VERSION << " | " << std::endl;
}

void Config::printSummary(const Config config) {
    std::cout << "Input  file Log:      " << config.inputLogFile << std::endl;
    std::cout << "Input  file Video:    " << config.inputVideoFile << std::endl;
    std::cout << "Output file:          " << (config.outputFile.empty() ? "None" : config.outputFile) << std::endl;
}

Config Config::parseCommandLine(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            config.showHelp = true;
            return config;
        } else if (arg == "-V" || arg == "--version") {
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
        } else if (arg == "-v" || arg == "--video") {
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
