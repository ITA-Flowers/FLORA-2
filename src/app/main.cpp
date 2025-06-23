#include <iostream>
#include <filesystem>
#include "Config.hpp"
#include "../core/NavProcessor.hpp"


int initNavProcessor(NavProcessor& navProcessor, const Config& config) {
    // Set camera parameters
    navProcessor.setCameraParams(config.getVideoFovCameraDeg(), {config.getVideoWidthPx(), config.getVideoHeightPx()});
    navProcessor.setFrameRate(config.getVideoFps());

    // Initialize input files
    if (navProcessor.initInput(std::filesystem::path(config.getInputDir())) != 0) {
        std::cerr << "Error: Could not initialize input files." << std::endl;
        return 1;
    }

    // Initialize output files
    if (navProcessor.initOutput(std::filesystem::path(config.getOutputDir())) != 0) {
        std::cerr << "Error: Could not initialize output files." << std::endl;
        return 2;
    }

    return 0;
}

int doProcessing(NavProcessor& navProcessor, const Config& config) {
    if (navProcessor.process() != 0) {
        std::cerr << "Error: Processing failed." << std::endl;
        return 3;
    }
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