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
    
    void setInputLogFile(const std::string& file) { inputLogFile = file; }
    
    void setInputVideoFile(const std::string& file) { inputVideoFile = file; }

    void setOutputFile(const std::string& file) { outputFile = file; }

private:
    const std::string APP_NAME = "FLORA-2";
    const std::string VERSION = "1.0.0";

    std::string inputLogFile;
    std::string inputVideoFile;
    std::string outputFile;
    bool showVersion;
    bool showHelp;
};