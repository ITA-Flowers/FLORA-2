function(fetch_dependencies)
    # =======================
    # Eigen3
    # =======================
    if(USE_EIGEN3)
        message(STATUS "Checking Eigen3...")

        if(EXISTS "${PROJECT_SOURCE_DIR}/include/external/eigen/Eigen/Core")
            message(STATUS "Using bundled Eigen headers")
            add_library(Eigen3::Eigen INTERFACE IMPORTED)
            set_target_properties(Eigen3::Eigen PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include/external/eigen"
            )
        else()
            find_package(Eigen3 3.3 REQUIRED)
        endif()
    else()
        message(STATUS "Eigen3 library is disabled")
    endif()

    # =======================
    # GoogleTest
    # =======================
    if(BUILD_TESTS)
        message(STATUS "Checking GoogleTest...")
        find_package(GTest REQUIRED)
    endif()

    # =======================
    # OpenCV
    # =======================
    if(USE_OPENCV)
        message(STATUS "Checking OpenCV...")
        find_package(OpenCV REQUIRED)
    else()
        message(STATUS "OpenCV is disabled")
    endif()

endfunction()
