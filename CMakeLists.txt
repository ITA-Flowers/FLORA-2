cmake_minimum_required(VERSION 3.15)

# Project
project(FLORA-2
    VERSION 1.0.0
    DESCRIPTION "FLORA - UAV GPS-Denied Navigation System /ver. 2"
    LANGUAGES CXX
)

# Options
option(BUILD_TESTS "Build the test suite" ON)
option(USE_EIGEN3 "Use Eigen3 library" ON)
option(ENABLE_WARNINGS "Enable all warnings" ON)

# Cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include dependencies
include(Dependencies)
fetch_dependencies()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings
if(ENABLE_WARNINGS)
    if(MSVC)
        # [?] Option `/WX` treats warnings as errors
        # add_compile_options(/W4 /WX)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# Add subdirectories
add_subdirectory(src)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
