// Config.hpp
#pragma once
#include <string>

class Config {

public:
    Config()
        : inputDir("")
        , outputDir("")
        , showVersion(false)
        , showHelp(false)
    {}

    static Config parseCommandLine(int argc, char* argv[]);

    void printHelp(const char* programName);

    void printVersion(void);

    void printSummary(const Config config);

    bool isShowVersion() const { return showVersion; }
    
    bool isShowHelp() const { return showHelp; }
    
    const std::string& getInputDir() const { return inputDir; }

    const std::string& getOutputDir() const { return outputDir; }

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
    
    void setInputDir(const std::string& dir) { inputDir = dir; }

    void setOutputFile(const std::string& file) { outputDir = file; }

private:
    const std::string APP_NAME = "FLORA-2";
    const std::string VERSION = "0.2.1";

    // Files
    std::string inputDir;
    std::string outputDir;
    
    // Video parameters
    int videoFps = 30; // default value
    int videoFovCameraDeg = 91; // default value
    int videoWidthPx = 1920; // default value
    int videoHeightPx = 1080; // default value
    int altitudeM = 100; // default value

    bool showVersion;
    bool showHelp;
};