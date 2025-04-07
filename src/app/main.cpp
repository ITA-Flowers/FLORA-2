// main.cpp
#include "Config.hpp"
#include <iostream>

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

        // * END OF PROGRAM LOGIC *
    }

    std::cerr << std::endl << "[DEBUG] Final return code: " << ret << std::endl;

    // - return code
    if (ret != 0) {
        std::cerr << "Error: Exit code: " << ret << std::endl;
    } 
    return ret;
}