# FLORA-2

A **dead reckoning** navigation system using **optical flow** for unmanned aerial vehicles (**UAV**).

>**NOTICE: *This project is private property and is protected by full copyright. All rights reserved.***

## Overview

This project aims to develop an advanced **dead reckoning** navigation system for drones, based on **optical flow** analysis. The system calculates course and speed by integrating data from multiple sources: dead reckoning algorithms, optical flow processing, and GPS (when available) using a complementary filter to determine the vehicle's current position.

The project primarily uses the [**AirSim**](https://github.com/microsoft/AirSim) simulator for development and testing. AirSim provides a realistic environment for algorithm development and validation before deploying to real hardware. In addition to simulation, test flights were also conducted using real drones ([**DJI Mini3**](https://www.dji.com/pl/mini-3/specs) and [**YUNEEC H520**](https://yuneec.online/h520-series/info/)) to validate the algorithms in real-world conditions.

## Features

- Processing of drone flight logs and video recordings

- Dead reckoning navigation implementation

- Optical flow based position tracking

- Complementary filtering for data fusion

- Position estimation with and without GPS

- Performance comparison between different navigation methods

- Validation against ground truth data

- AirSim simulator integration for development and testing

- Support for both simulated and real-world flight data

## Project Architecture

``` text
.
├ cmake/           # CMake dependencies
├ data/            # Test data
├ docs/            # Documentation
├ src/             # Source code (C++ with CMake)
├ tests/           # Unit tests
└ tools/           # Utility scripts (mostly Python)
```

## Requirements

|Requirement|Version|
|-|-|
|*[AirSim](https://github.com/microsoft/AirSim)* with **Unreal Engine**|4.27|
|**C++ compiler**|17|
|**CMake**| 3.12+|
|**OpenCV**| 4.9.0|
|**Eigen**| 3.3+|
|**Boost**| 1.65+|
|**Python**| 3.9|

Additional dependencies will be listed in `CMakeLists.txt`

## Building

### Prerequisite

1. Make sure all dependencies are installed

2. Clone the repository

``` bash
git clone https://github.com/ITA-Flowers/FLORA-2.git
cd FLORA-2
```

### Basic Build

``` bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build . -j$(nproc)  # On Linux/macOS
# or
cmake --build . -j%NUMBER_OF_PROCESSORS%  # On Windows
```

### Build Options

The following CMake options are available:

- `BUILD_TESTS=ON/OFF` - Build unit and integration tests (default: ON)

- `USE_EIGEN=ON/OFF` - Fetch Eigen3 library if needed (default: ON)

- `ENABLE_WARNINGS=ON/OFF` - Enable all warning (default: ON)

Example:

``` bash
cmake -DBUILD_TESTS=ON -DUSE_EIGEN=OFF ..
```

### Resetting the Build Configuration

If you need to completely reset your build configuration (e.g., after changing compilers or major dependencies):

``` bash
# From the build directory, remove all generated files
rm -rf *  # On Linux/macOS
# or
del /s /q *  # On Windows, or simply delete the folder contents

# Reconfigure from scratch
cmake ..
```

Alternatively, you can create a fresh build directory:

``` bash
# From the project root
rm -rf build  # On Linux/macOS (or delete it manually on Windows)
mkdir build && cd build
cmake ..
```

For a "clean build" without resetting the configuration:

``` bash
# From the build directory
cmake --build . --target clean
# Then rebuild
cmake --build .
```

### Build Configurations

``` bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with specific compiler
cmake -DCMAKE_CXX_COMPILER=g++-10 ..
```

## Developer Workflow

### Programming

1. Create a new branch for your feature:

```  bash
git checkout develop
git pull
git checkout -b feature/feature-name
```

2. Work on your code, commit changes:

``` bash
git add .
git commit -m "Descriptive commit message"
```

3. Push your branch and create a pull request:

``` bash
git push origin feature/feature-name
```

4. Create a pull request to the develop branch in GitHub interface.

### Code Review

1. Assign at least one reviewer to your pull request

2. Address all comments and suggestions

3. Make requested changes and push additional commits

4. Request re-review after addressing feedback

5. PR will be merged by the reviewer after approval

### Reverting Changes

If a change needs to be reverted:

1. For uncommitted changes:

``` bash
git checkout -- path/to/file
# Or to discard all changes
git restore .
```

2. For committed changes that haven't been pushed:

``` bash
git reset HEAD~1    # Preserves changes in working directory
# Or
git reset --hard HEAD~1    # Discards changes completely
```

3. For changes that have been pushed:

``` bash
git revert <commit-hash>
git push origin <branch-name>
```

4. For reverting an entire feature branch after merge:

    - Create a new PR that reverts the merge commit
    - Document why the reversion was necessary

---
## Example results
Trajectory:
![Obraz 23 06 2025, godz  10 01 (1)](https://github.com/user-attachments/assets/70eb3b70-56ca-49f5-8a05-ef5433b84207)

Velocity
![Obraz 23 06 2025, godz  10 01](https://github.com/user-attachments/assets/056f3532-90aa-4298-9597-3d56d0692be0)


## Project Status

The project is in the early development phase. We are currently implementing the core components of the dead reckoning and optical flow algorithms.

## License

### Copyright © 2025. All rights reserved.

This research project is protected by full copyright and is not available for use, copying, modification, or distribution without express written permission from the owner.
