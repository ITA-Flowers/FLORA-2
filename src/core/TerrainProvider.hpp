#pragma once

#include <limits>
#include <opencv2/core.hpp>
#include <string>

class TerrainProvider {
public:
    TerrainProvider();
    ~TerrainProvider();

    TerrainProvider(const TerrainProvider&) = delete;
    TerrainProvider& operator=(const TerrainProvider&) = delete;

    bool load(const std::string& rasterPath);
    bool sampleLatLon(double latitudeDeg, double longitudeDeg, double& heightMeters) const;
    bool isLoaded() const { return !dataset_.empty(); }

private:
    bool transformLatLonToRasterCrs(double latitudeDeg, double longitudeDeg, double& x, double& y) const;

    double geoTransform_[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    cv::Mat dataset_;
    int rasterWidth_ = 0;
    int rasterHeight_ = 0;
    float noDataValue_ = std::numeric_limits<float>::quiet_NaN();
    bool hasNoData_ = false;
    std::string targetEpsg_ = "EPSG:2180";
};
