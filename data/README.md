# Data

This directory contains test data used for development, testing, and validation of the UAV Optical Flow Navigation System.

## Directory Structure

- `/logs` - Flight logs from test flights

    - Format: CSV or proprietary formats from different drone manufacturers

    - Naming convention: "`src`\_`flight_id`\_`date`.`extension` (src: airsim, dji, yuneec, etc.) (date format: "yyyy-mm-dd")

    - Contains: IMU readings, GPS coordinates (if available), barometric data, etc.


- `/video` - Video recordings from drone cameras

    - Format: MP4 or other common video formats

    - Naming convention: "`src`\_`flight_id`\_`date`.`extension` 

    - Resolution and frame rate should be noted in the filename or in a metadata file


- `/reference` - Reference datasets for validation

    - `/ground_truth` - High-precision position data (e.g., from RTK GPS)
    
    - `/benchmarks` - Standard benchmark datasets
    
    - `/calibration` - Sensor calibration data


- `/airsim` - AirSim simulation data

## Adding New Data

When adding new test data, please follow these guidelines:

1. Place the data in the appropriate subdirectory

2. Follow the naming conventions

3. Create a metadata file alongside each dataset (as a .json or .txt file) with:

    - Date and time of recording

    - Equipment used (drone model, camera specifications)

    - Environmental conditions (indoor/outdoor, lighting, weather)

    - Any known issues or anomalies in the data

    - Copyright or usage restrictions (if applicable)

## Data Preprocessing

Raw data often needs preprocessing before it can be used with the system. See the `/tools/preprocessing` directory for scripts to help with this process.

## Storage Considerations

Due to the potentially large size of video files, this repository does not directly store large datasets. Instead:

- Small sample datasets are stored directly in the repository

- Larger datasets are stored externally and referenced by URL in the metadata files

- The project's CI pipeline will download necessary test data automatically

## Data Formats

**IMU logs**: **CSV** with columns for `timestamp`, `accelerometer` (x,y,z), `gyroscope` (x,y,z), `magnetometer` (x,y,z), `barometer` (x,y,z), 

**GPS logs**: **CSV** with columns for `timestamp`, `latitude`, `longitude`, `altitude`, `accuracy`

**Video**: **MP4** or **AVI** format, preferably with **constant frame rate** (30 FPS)

## Data Privacy

Ensure that all contributed data does not contain sensitive information such as exact coordinates of private locations. When in doubt, apply appropriate anonymization techniques.