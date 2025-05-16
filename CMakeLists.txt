cmake_minimum_required(VERSION 3.15)

# ========================
# Project Metadata
# ========================
project(FLORA-2
    VERSION 1.0.0
    DESCRIPTION "FLORA - UAV GPS-Denied Navigation System / ver. 2"
    LANGUAGES CXX
)

# ========================
# Options
# ========================
option(BUILD_TESTS "Build the test suite" OFF)
option(USE_EIGEN3 "Use Eigen3 library" ON)
option(USE_OPENCV "Use OpenCV library" ON)
option(ENABLE_WARNINGS "Enable all warnings" ON)

# ========================
# Module Path
# ========================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ========================
# Dependencies
# ========================
include(Dependencies)  # Includes fetch_dependencies()
fetch_dependencies()

# ========================
# Compiler Settings
# ========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ========================
# Compiler Warnings
# ========================
if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# ========================
# Source Code
# ========================
add_subdirectory(src)

# ========================
# Tests
# ========================
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
