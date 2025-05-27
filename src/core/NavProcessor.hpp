#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cmath>

#include "../nav-dr/core/DeadReckoningProcessor.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"

class NavProcessor {
public:
    NavProcessor() = default;

    void setCameraParams(int fovDeg, const std::pair<int, int>& resolution) {
        opticalFlowProcessor_.setCameraParams(fovDeg, resolution);
    }

    void setFrameRate(int fps) {
        opticalFlowProcessor_.setFrameRate(fps);
    }

    int initInput(const std::filesystem::path& inputDir);

    int initOutput(const std::filesystem::path& outputDir);

    int process(void);

private:
    int countLinesInFile(const std::filesystem::path& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file: " << filePath << std::endl;
            return -1;
        }
        int lines = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
        file.close();
        return lines;
    }

    OpticalFlowProcessor opticalFlowProcessor_;
    DeadReckoningProcessor deadReckoningProcessor_;

    std::string fileBasename_;
    std::filesystem::path inputLogFile_;
    std::filesystem::path inputGPSFile_;
    std::filesystem::path inputVideoFile_;
    std::filesystem::path outputLogFile_;
};