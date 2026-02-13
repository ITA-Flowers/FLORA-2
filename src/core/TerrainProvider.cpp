#include "TerrainProvider.hpp"

#include <array>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <opencv2/imgcodecs.hpp>

TerrainProvider::TerrainProvider() = default;
TerrainProvider::~TerrainProvider() = default;

bool TerrainProvider::load(const std::string& rasterPath) {
    dataset_ = cv::imread(rasterPath, cv::IMREAD_UNCHANGED);
    if (dataset_.empty()) {
        std::cerr << "Warning: Cannot open DEM raster: " << rasterPath << std::endl;
        return false;
    }

    if (dataset_.channels() != 1) {
        std::cerr << "Warning: DEM raster must be single-band: " << rasterPath << std::endl;
        dataset_.release();
        return false;
    }

    rasterWidth_ = dataset_.cols;
    rasterHeight_ = dataset_.rows;
    hasNoData_ = false;
    targetEpsg_ = "EPSG:2180";

    std::string infoCmd = "gdalinfo \"" + rasterPath + "\"";
    FILE* pipe = popen(infoCmd.c_str(), "r");
    if (pipe == nullptr) {
        std::cerr << "Warning: Cannot execute gdalinfo for: " << rasterPath << std::endl;
        dataset_.release();
        return false;
    }

    std::string info;
    std::array<char, 4096> buffer{};
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        info += buffer.data();
    }
    pclose(pipe);

    const std::regex originRegex(R"(Origin = \(([-+0-9.eE]+),\s*([-+0-9.eE]+)\))", std::regex::icase);
    const std::regex pixelRegex(R"(Pixel Size = \(([-+0-9.eE]+),\s*([-+0-9.eE]+)\))", std::regex::icase);
    std::smatch originMatch;
    std::smatch pixelMatch;
    if (!std::regex_search(info, originMatch, originRegex) || originMatch.size() < 3 ||
        !std::regex_search(info, pixelMatch, pixelRegex) || pixelMatch.size() < 3) {
        std::cerr << "Warning: Cannot parse DEM geotransform from gdalinfo: " << rasterPath << std::endl;
        dataset_.release();
        return false;
    }

    const double originX = std::stod(originMatch[1].str());
    const double originY = std::stod(originMatch[2].str());
    const double pixelSizeX = std::stod(pixelMatch[1].str());
    const double pixelSizeY = std::stod(pixelMatch[2].str());

    geoTransform_[0] = originX;
    geoTransform_[1] = pixelSizeX;
    geoTransform_[2] = 0.0;
    geoTransform_[3] = originY;
    geoTransform_[4] = 0.0;
    geoTransform_[5] = pixelSizeY;

    const std::regex nodataRegex(R"(NoData Value=\s*([-+0-9.eE]+))", std::regex::icase);
    std::smatch nodataMatch;
    if (std::regex_search(info, nodataMatch, nodataRegex) && nodataMatch.size() >= 2) {
        hasNoData_ = true;
        noDataValue_ = std::stof(nodataMatch[1].str());
    }

    const std::regex epsgRegex(R"epsg(ID\["EPSG",\s*"([0-9]+)"\])epsg", std::regex::icase);
    std::smatch epsgMatch;
    if (std::regex_search(info, epsgMatch, epsgRegex) && epsgMatch.size() >= 2) {
        targetEpsg_ = "EPSG:" + epsgMatch[1].str();
    }

    return true;
}

bool TerrainProvider::transformLatLonToRasterCrs(
    double latitudeDeg, double longitudeDeg, double& x, double& y) const {
    std::ostringstream cmd;
    cmd << "echo \"" << longitudeDeg << " " << latitudeDeg
        << "\" | gdaltransform -s_srs EPSG:4326 -t_srs " << targetEpsg_ << " 2>/dev/null";

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (pipe == nullptr) {
        return false;
    }

    std::array<char, 512> buffer{};
    std::string output;
    if (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output = buffer.data();
    }
    pclose(pipe);

    std::istringstream ss(output);
    double zIgnored = 0.0;
    if (!(ss >> x >> y >> zIgnored)) {
        return false;
    }
    return true;
}

bool TerrainProvider::sampleLatLon(double latitudeDeg, double longitudeDeg, double& heightMeters) const {
    if (dataset_.empty()) {
        return false;
    }

    double x = 0.0;
    double y = 0.0;
    if (!transformLatLonToRasterCrs(latitudeDeg, longitudeDeg, x, y)) {
        return false;
    }

    const double pixel = (x - geoTransform_[0]) / geoTransform_[1];
    const double line = (y - geoTransform_[3]) / geoTransform_[5];
    const int col = static_cast<int>(std::floor(pixel));
    const int row = static_cast<int>(std::floor(line));

    if (col < 0 || row < 0 || col >= rasterWidth_ || row >= rasterHeight_) {
        return false;
    }

    float sample = std::numeric_limits<float>::quiet_NaN();
    switch (dataset_.depth()) {
        case CV_32F:
            sample = dataset_.at<float>(row, col);
            break;
        case CV_64F:
            sample = static_cast<float>(dataset_.at<double>(row, col));
            break;
        case CV_16U:
            sample = static_cast<float>(dataset_.at<uint16_t>(row, col));
            break;
        case CV_16S:
            sample = static_cast<float>(dataset_.at<int16_t>(row, col));
            break;
        case CV_8U:
            sample = static_cast<float>(dataset_.at<uint8_t>(row, col));
            break;
        default:
            return false;
    }

    if (std::isnan(sample)) {
        return false;
    }
    if (hasNoData_ && std::abs(sample - noDataValue_) <= 1e-5f) {
        return false;
    }

    heightMeters = static_cast<double>(sample);
    return true;
}
