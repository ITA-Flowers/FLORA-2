cmake_minimum_required(VERSION 3.10)
project(nav_of)

set(CMAKE_CXX_STANDARD 17)

# Znajdź OpenCV z CUDA (zakładamy, że instalacja trafiła do /usr/local/lib/cmake/opencv4)
set(OpenCV_DIR /usr/local/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/algo
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algo/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp
)

add_executable(nav_of_main ${SOURCES})

# 🧠 Linkowanie pełnej biblioteki OpenCV (z CUDA)
target_link_libraries(nav_of_main ${OpenCV_LIBS})
