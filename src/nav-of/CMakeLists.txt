cmake_minimum_required(VERSION 3.10)
project(RealtimeOpticalFlow)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} src)

add_executable(realtime_optical_flow
    src/main-of.cpp
    src/optical_flow_realtime.cpp
    src/horn_schunck.cpp
    src/kalman_filter.cpp
    src/utils.cpp
)

target_link_libraries(realtime_optical_flow ${OpenCV_LIBS})
