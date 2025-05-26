// Config.hpp
#pragma once
#include <string>

class Config {

public:
    Config()
        : inputLogFile("")
        , inputVideoFile("")
        , outputFile("")
        , showVersion(false)
        , showHelp(false)
    {}

    static Config parseCommandLine(int argc, char* argv[]);

    void printHelp(const char* programName);

    void printVersion(void);

    void printSummary(const Config config);

    bool isShowVersion() const { return showVersion; }
    
    bool isShowHelp() const { return showHelp; }
    
    const std::string& getInputLogFile() const { return inputLogFile; }
    
    const std::string& getInputVideoFile() const { return inputVideoFile; }

    const std::string& getOutputFile() const { return outputFile; }

    int getVideoFps() const { return videoFps; }

    int getVideoFovCameraDeg() const { return videoFovCameraDeg; }

    int getVideoWidthPx() const { return videoWidthPx; }

    int getVideoHeightPx() const { return videoHeightPx; }

    int getAltitudeM() const { return altitudeM; }

    void setVideoFps(int fps) { videoFps = fps; }

    void setVideoFovCameraDeg(int fov) { videoFovCameraDeg = fov; }

    void setVideoWidthPx(int width) { videoWidthPx = width; }

    void setVideoHeightPx(int height) { videoHeightPx = height; }

    void setAltitudeM(int altitude) { altitudeM = altitude; }
    
    void setInputLogFile(const std::string& file) { inputLogFile = file; }
    
    void setInputVideoFile(const std::string& file) { inputVideoFile = file; }

    void setOutputFile(const std::string& file) { outputFile = file; }

private:
    const std::string APP_NAME = "FLORA-2";
    const std::string VERSION = "0.1.0";

    // std::cerr << "Usage: " << argv[0] << " <video_path> <fps> <altitude_m> <fov_camera_deg> <video_width_px> <video_height_px>\n";

    // Files
    std::string inputLogFile;
    std::string inputVideoFile;
    std::string outputFile;
    
    // Video parameters
    int videoFps = 30; // default value
    int videoFovCameraDeg = 91; // default value
    int videoWidthPx = 1920; // default value
    int videoHeightPx = 1080; // default value
    int altitudeM = 100; // default value

    bool showVersion;
    bool showHelp;
};