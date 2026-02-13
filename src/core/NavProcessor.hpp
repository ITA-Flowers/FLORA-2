#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <numeric>

#include "../nav-dr/core/DeadReckoningProcessor.hpp"
#include "../nav-of/core/OpticalFlowProcessor.hpp"
#include "TerrainProvider.hpp"

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
    size_t countLinesInFile(const std::string& filePath) {
        std::ifstream file(filePath);
        size_t lines = 0;
        std::string unused;

        while (std::getline(file, unused)) {
            ++lines;
        }

        return lines;
    }

    double computeFrequencyFromTimestamps(const std::filesystem::path& csvFile, const std::string& columnName);

    OpticalFlowProcessor opticalFlowProcessor_;
    DeadReckoningProcessor deadReckoningProcessor_;
    TerrainProvider nmtProvider_;
    TerrainProvider nmptProvider_;

    bool terrainOriginInitialized_ = false;
    double hGround0_ = 0.0;
    double hSurface0_ = 0.0;
    double lastSelectedAgl_ = 0.0;
    bool hasLastSelectedAgl_ = false;
    bool preferSurface_ = true;
    int terrainSwitchDebounce_ = 0;

    std::string fileBasename_;
    std::filesystem::path inputLogFile_;
    std::filesystem::path inputGPSFile_;
    std::filesystem::path inputVideoFile_;
    std::filesystem::path outputLogFile_;
};
