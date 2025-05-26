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

    inputVideoFile_ = inputDir / ("video_" + fileBasename_.substr(4) + ".mp4");

    if (!std::filesystem::exists(inputLogFile_)) {
        std::cerr << "Error: Input log file does not exist: " << inputLogFile_ << std::endl;
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
    std::cout << "    - opening input files:\n      - " << inputLogFile_ << ",\n      - " << inputVideoFile_ << std::endl;
    std::ifstream inFile(inputLogFile_);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open input log file: " << inputLogFile_ << std::endl;
        return -1;
    }
    std::cout << "      * input files opened successfully." << std::endl;

    // Read the header line from the log file
    std::cout << "    - reading header from input log file: " << inputLogFile_ << std::endl;
    std::string headerLine;
    if (!std::getline(inFile, headerLine)) {
        std::cerr << "Error: Could not read header from input log file." << std::endl;
        inFile.close();
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
        return -1;
    }
    
    std::cout << "      * header read successfully." << std::endl;

    // Open video file
    std::cout << "    - opening video file: " << inputVideoFile_ << std::endl;
    cv::VideoCapture cap(inputVideoFile_.string());
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file: " << inputVideoFile_ << std::endl;
        inFile.close();
        return -1;
    }
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
    outFile << "frame_number,speed_mps,altitude,heading,dr_lat,dr_lon,gps_lat,gps_lon\n";
    std::cout << "      * header written successfully." << std::endl;

    // Process video frames and log data
    std::cout << "    - processing:" << std::endl;
    cv::Mat frame;
    int frameCount = 0;

    while (cap.read(frame) && !inFile.eof()) {
        std::string line;
        if (!std::getline(inFile, line)) break;

        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> values;

        while (std::getline(lineStream, cell, ',')) {
            values.push_back(cell);
        }

        double ref_timestamp = std::stod(values[columnIndex["ref_timestamp"]]);
        double ref_lat = std::stod(values[columnIndex["ref_lat"]]);
        double ref_lon = std::stod(values[columnIndex["ref_lon"]]);

        double timestamp = std::stod(values[columnIndex["timestamp"]]);
        double vx = std::stod(values[columnIndex["vx"]]);
        double vy = std::stod(values[columnIndex["vy"]]);
        double alt = -std::stod(values[columnIndex["z"]]);

        double heading_rad = std::atan2(vx, vy);
        double heading_deg = heading_rad * 180.0 / M_PI;
        if (heading_deg < 0) {
            heading_deg += 360.0; // Normalize to [0, 360)
        }

        frameCount++;
        if (!opticalFlowProcessor_.update(frame, alt)) {
            std::cerr << "Error: Optical flow update failed for frame " << frameCount << "." << std::endl;
            continue;
        }

        Vector3D velocity = opticalFlowProcessor_.getVelocity();
        double speed_mps = velocity.getX();

        if (!deadReckoningProcessor_.update(
                deadReckoningProcessor_.getGPSData(),
                alt,
                heading_rad,
                speed_mps,
                1.0 / opticalFlowProcessor_.getFrameRate())) {
            std::cerr << "Error: Dead reckoning update failed for frame " << frameCount << "." << std::endl;
            continue;
        }

        // Hint: headers: frame_number | speed_mps | altitude | heading | dr_lat | dr_lon | gps_lat | gps_lon
        GPSData gpsData = deadReckoningProcessor_.getGPSData();
        outFile << frameCount << ","
                << speed_mps << ","
                << alt << ","
                << heading_deg << ","
                << gpsData.getLatitude() << ","
                << gpsData.getLongitude() << ","
                << ref_lat << ","
                << ref_lon << "\n";

        if (frameCount != 1) {
            std::cout << "\033[8A";
            for (int i = 0; i < 8; ++i) std::cout << "\033[2K\033[1B";
            std::cout << "\033[8A";
        }
        
        std::cout << "frame:    " << frameCount << "\n"
                << "speed:    " << speed_mps << " m/s\n"
                << "altitude: " << alt << " m\n"
                << "heading:  " << heading_deg << " deg\n"
                << "dr_lat:   " << gpsData.getLatitude() << "\n"
                << "dr_lon:   " << gpsData.getLongitude() << "\n"
                << "gps_lat:  " << ref_lat << "\n"
                << "gps_lon:  " << ref_lon << "\n"
                << std::flush;
    }

    outFile.close();
    inFile.close();
    cap.release();

    return 0;
}