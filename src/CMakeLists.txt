# Creating the application components library
# -- Flora App
add_library(flora_app
    app/Config.cpp
)

target_include_directories(flora_app
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# -- Flore Core
add_library(flora_core
    core/Vector3D.cpp
    core/Quaternion.cpp
    core/Matrix.cpp
)

target_include_directories(flora_core
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

if(USE_EIGEN3)
    target_link_libraries(flora_core
        PUBLIC
            Eigen3::Eigen
    )
endif()

# -- Flora Nav-DR (Dead Reckoning)
add_library(flora_nav-dr
    nav-dr/sensors/GPSData.cpp
    nav-dr/sensors/IMUData.cpp
    nav-dr/sensors/SensorData.cpp
)

target_include_directories(flora_nav-dr
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# -- Flora Nav-OF (Optical Flow)
set(OpenCV_DIR /usr/local/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

add_library(flora_nav-of
    nav-of/algo/farneback_gpu.cpp
    nav-of/algo/horn_schunck.cpp
    nav-of/algo/utils.cpp
    nav-of/core/OpticalFlowProcessor.cpp
)

target_include_directories(flora_nav-of
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        ${OpenCV_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# Adding the main application executable
add_executable(flora2
    app/main.cpp
)

# Linking the application with the components library
target_link_libraries(flora2
    PRIVATE
        flora_app
        flora_core
        flora_nav-of
        ${OpenCV_LIBS}
        # flora_nav-dr
        # flora_nav-sf
)