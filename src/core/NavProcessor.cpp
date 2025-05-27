#include "NavProcessor.hpp"

int NavProcessor::initInput(const std::filesystem::path& inputDir) {
    std::string dirStr = inputDir.string();
    if (!dirStr.empty() && dirStr.back() == '/') {
        dirStr.pop_back();
    }

    std::filesystem::path cleaned(dirStr);
    fileBasename_ = cleaned.filename().string();

    if (fileBasename_.empty()) {
        std::cerr << "Error: Input directory name is empty." << std::endl;
        return -1;
    }

    std::filesystem::path logSubdir = inputDir / (fileBasename_ + "_converted_trimmed");
    std::filesystem::path logFilename = fileBasename_ + "_vehicle_local_position_0.csv";
    inputLogFile_ = logSubdir / logFilename;

    std::filesystem::path gpsLogFilename = fileBasename_ + "_vehicle_gps_position_0.csv";
    inputGPSFile_ = logSubdir / gpsLogFilename;

    inputVideoFile_ = inputDir / ("video_" + fileBasename_.substr(4) + ".mp4");

    if (!std::filesystem::exists(inputLogFile_)) {
        std::cerr << "Error: Input log file does not exist: " << inputLogFile_ << std::endl;
        return -1;
    }

    if (!std::filesystem::exists(inputGPSFile_)) {
        std::cerr << "Error: Input GPS file does not exist: " << inputGPSFile_ << std::endl;
        return -1;
    }

    if (!std::filesystem::exists(inputVideoFile_)) {
        std::cerr << "Error: Input video file does not exist: " << inputVideoFile_ << std::endl;
        return -1;
    }

    return 0;
}

int NavProcessor::initOutput(const std::filesystem::path& outputDir) {
    if (fileBasename_.empty()) {
        std::cerr << "Error: File basename is empty. Please initialize input first." << std::endl;
        return -1;
    }

    if (!std::filesystem::exists(outputDir)) {
        std::cerr << "Error: Output directory does not exist." << std::endl;
        return -1;
    }

    outputLogFile_ = outputDir / (fileBasename_ + ".csv");

    return 0;
}


int NavProcessor::process(void) {
    // Check if input files are initialized
    std::cout << "    - checking input files: ";
    if (inputLogFile_.empty() || inputVideoFile_.empty()) {
        std::cerr << "Error: Input files are not initialized." << std::endl;
        return -1;
    }
    std::cout << "OK" << std::endl;

    // Open input files
    std::cout << "    - opening input files:\n";
    int logLines = int(countLinesInFile(inputLogFile_.string()));
    std::cout << "      * input log file: " << inputLogFile_ << " | lines: " << logLines << std::endl;
    if (logLines < 0) {
        std::cerr << "Error: Could not count lines in input log file." << std::endl;
        return -1;
    }

    int gpsLines = int(countLinesInFile(inputGPSFile_.string()));
    std::cout << "      * input GPS file: " << inputGPSFile_ << " | lines: " << gpsLines << std::endl;
    if (gpsLines < 0) {
        std::cerr << "Error: Could not count lines in input GPS file." << std::endl;
        return -1;
    }

    std::ifstream inFile(inputLogFile_);
    std::ifstream gpsFile(inputGPSFile_);

    std::cout << "      * input files opened successfully." << std::endl;

    // Read the header line from the log file
    std::cout << "    - reading header from input log file: " << inputLogFile_ << std::endl;
    std::string headerLine;
    if (!std::getline(inFile, headerLine)) {
        std::cerr << "Error: Could not read header from input log file." << std::endl;
        inFile.close();
        gpsFile.close();
        return -1;
    }
    std::unordered_map<std::string, size_t> columnIndex;
    std::vector<std::string> headers;
    std::stringstream ss(headerLine);
    size_t idx = 0;
    std::string column;
    
    while (std::getline(ss, column, ',')) {
        columnIndex[column] = idx++;
    }
    
    if (columnIndex.count("vx") == 0 || columnIndex.count("vy") == 0 || columnIndex.count("z") == 0) {
        std::cerr << "Error: Required columns not found in CSV header." << std::endl;
        inFile.close();
        gpsFile.close();
        return -1;
    }

    // Read the header line from the GPS log file
    std::cout << "    - reading header from gps log file: " << inputGPSFile_ << std::endl;
    std::string gpsHeaderLine;
    if (!std::getline(gpsFile, gpsHeaderLine)) {
        std::cerr << "Error: Could not read header from gps log file." << std::endl;
        inFile.close();
        gpsFile.close();
        return -1;
    }
    std::unordered_map<std::string, size_t> gpsColumnIndex;
    std::vector<std::string> gpsHeaders;
    std::stringstream ssg(gpsHeaderLine);
    size_t idxg = 0;
    std::string gpsColumn;
    
    while (std::getline(ssg, gpsColumn, ',')) {
        gpsColumnIndex[gpsColumn] = idxg++;
    }
    
    if (gpsColumnIndex.count("lat") == 0 || gpsColumnIndex.count("lon") == 0 || gpsColumnIndex.count("vel_m_s") == 0) {
        std::cerr << "Error: Required columns not found in GPS CSV header." << std::endl;
        return -1;
    }
    
    std::cout << "      * headers read successfully." << std::endl;

    // Open video file
    std::cout << "    - opening video file: " << inputVideoFile_ << std::endl;
    cv::VideoCapture cap(inputVideoFile_.string());
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file: " << inputVideoFile_ << std::endl;
        inFile.close();
        return -1;
    }

    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    opticalFlowProcessor_.setFrameRate(static_cast<int>(cap.get(cv::CAP_PROP_FPS)));

    std::cout << "        - frame rate: " << opticalFlowProcessor_.getFrameRate() << " fps" << std::endl;
    std::cout << "        - total frames: " << totalFrames << std::endl;
    std::cout << "      * video file opened successfully." << std::endl;

    // Open output file
    std::cout << "    - opening output file: " << outputLogFile_ << std::endl;
    std::ofstream outFile(outputLogFile_);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputLogFile_ << std::endl;
        return -1;
    }
    std::cout << "      * output file opened successfully." << std::endl;

    // Write header to output file
    std::cout << "    - writing header to output file." << std::endl;
    std::cout << std::fixed << std::setprecision(10);
    outFile << std::fixed << std::setprecision(10);
    outFile << "frame_number,speed_mps,altitude,heading,dr_lat,dr_lon,gps_lat,gps_lon,gps_vel\n";
    std::cout << "      * header written successfully." << std::endl;

    // Process video frames and log data
    std::cout << "    - processing:" << std::endl;
    cv::Mat frame;
    int frameCount = 0;

    /*
     * logLines: number of lines in the log file
     * gpsLines: number of lines in the GPS file
     * totalFrames: number of frames in the video file
     * 
     * 
    */

    int minSamples = std::min({logLines, gpsLines, totalFrames});

    int logFactor   = std::round(static_cast<double>(logLines) / static_cast<double>(minSamples));
    int gpsFactor   = std::round(static_cast<double>(gpsLines) / static_cast<double>(minSamples));
    int videoFactor = std::round(static_cast<double>(totalFrames) / static_cast<double>(minSamples));
    std::cout << "      * log factor: " << logFactor << "\n"
              << "      * gps factor: " << gpsFactor << "\n"
              << "      * video factor: " << videoFactor << "\n"
              << std::endl;

    int logFactorCounter = 0;
    int gpsFactorCounter = 0;
    int videoFactorCounter = 0;

    std::string line;

    std::cout << "\n\n\n\n\n\n\n\n" << std::endl;
    while (cap.read(frame) && !inFile.eof() && !gpsFile.eof()) {

        // -----------------------------------------------------------------------------------------------------
        // * Log file processing
        std::string line;
        if (!std::getline(inFile, line)) break;

        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> values;

        while (std::getline(lineStream, cell, ',')) {
            values.push_back(cell);
        }

        double timestamp = std::stod(values[columnIndex["timestamp"]]);
        double vx = std::stod(values[columnIndex["vx"]]);
        double vy = std::stod(values[columnIndex["vy"]]);
        double alt = -std::stod(values[columnIndex["z"]]);

        double heading_rad = std::atan2(vx, vy);
        double heading_deg = heading_rad * 180.0 / M_PI;
        if (heading_deg < 0) {
            heading_deg += 360.0; // Normalize to [0, 360)
        }

        // -----------------------------------------------------------------------------------------------------
        // * GPS file processing
        std::string gpsLine;
        if (!std::getline(gpsFile, gpsLine)) break;

        std::stringstream gpsLineStream(gpsLine);
        std::string gpsCell;
        std::vector<std::string> gpsValues;

        while (std::getline(gpsLineStream, gpsCell, ',')) {
            gpsValues.push_back(gpsCell);
        }

        double ref_lat = std::stoi(gpsValues[gpsColumnIndex["lat"]]) / 1e7;
        double ref_lon = std::stoi(gpsValues[gpsColumnIndex["lon"]]) / 1e7;
        double ref_vel_m_s = std::stod(gpsValues[gpsColumnIndex["vel_m_s"]]);

        // -----------------------------------------------------------------------------------------------------
        // * Frame processing
        frameCount++;
        if (!opticalFlowProcessor_.update(frame, alt)) {
            std::cerr << "Error: Optical flow update failed for frame " << frameCount << "." << std::endl;
            continue;
        }

        Vector3D velocity = opticalFlowProcessor_.getVelocity();
        double speed_mps = velocity.getX();

        // * Update dead reckoning processor
        if (!deadReckoningProcessor_.update(
                GPSData(ref_lat, ref_lon, alt),
                alt,
                heading_rad,
                speed_mps,
                1.0 / opticalFlowProcessor_.getFrameRate())) {
            std::cerr << "Error: Dead reckoning update failed for frame " << frameCount << "." << std::endl;
            continue;
        }

        // -----------------------------------------------------------------------------------------------------
        // * Get dead reckoning GPS data and write to output file
        // ? Hint: headers: frame_number | speed_mps | altitude | heading | dr_lat | dr_lon | gps_lat | gps_lon
        GPSData gpsData = deadReckoningProcessor_.getGPSData();
        outFile << frameCount << ","
                << speed_mps << ","
                << alt << ","
                << heading_deg << ","
                << gpsData.getLatitude() << ","
                << gpsData.getLongitude() << ","
                << ref_lat << ","
                << ref_lon << ","
                << ref_vel_m_s << "\n";

        if (frameCount != 1) {
            std::cout << "\033[9A";
            for (int i = 0; i < 9; ++i) std::cout << "\033[2K\033[1B";
            std::cout << "\033[9A";
        }
        
        std::cout << "      frame:    " << frameCount << " / " << totalFrames << "\n"
                << "      speed:    " << speed_mps << " m/s\n"
                << "      altitude: " << alt << " m\n"
                << "      heading:  " << heading_deg << " deg\n"
                << "      dr_lat:   " << gpsData.getLatitude() << "\n"
                << "      dr_lon:   " << gpsData.getLongitude() << "\n"
                << "      gps_lat:  " << ref_lat << "\n"
                << "      gps_lon:  " << ref_lon << "\n"
                << "      gps_vel:  " << ref_vel_m_s << " m/s\n"
                << std::flush;
    }

    outFile.close();
    inFile.close();
    cap.release();

    return 0;
}