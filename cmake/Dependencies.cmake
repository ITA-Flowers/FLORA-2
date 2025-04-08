include(FetchContent)

function(fetch_dependencies)
    # =======================
    # Eigen3
    # =======================
    if(USE_EIGEN3)
        message(STATUS "Fetching Eigen3...")

        if(EXISTS "${PROJECT_SOURCE_DIR}/include/external/eigen/Eigen/Core")
            message(STATUS "Using local Eigen installation")
            add_library(Eigen3::Eigen INTERFACE IMPORTED)
            set_target_properties(Eigen3::Eigen PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include/external/eigen"
            )
        else()
            find_package(Eigen3 3.3 QUIET)
            if(NOT Eigen3_FOUND)
                message(STATUS "Eigen not found. Downloading...")
                FetchContent_Declare(
                    eigen
                    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
                    GIT_TAG 3.4.0
                )
                FetchContent_MakeAvailable(eigen)

                if(NOT TARGET Eigen3::Eigen)
                    add_library(Eigen3::Eigen INTERFACE IMPORTED)
                    set_target_properties(Eigen3::Eigen PROPERTIES
                        INTERFACE_INCLUDE_DIRECTORIES "${eigen_SOURCE_DIR}"
                    )
                endif()
            endif()
        endif()
    else()
        message(STATUS "Eigen3 library is disabled")
    endif()

    # =======================
    # GoogleTest
    # =======================
    if(BUILD_TESTS)
        message(STATUS "Fetching GoogleTest...")

        find_package(GTest QUIET)
        if(NOT GTest_FOUND)
            message(STATUS "GoogleTest not found. Downloading...")
            FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG release-1.12.1
            )
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
            set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(googletest)
        endif()
    endif()

    # =======================
    # OpenCV
    # =======================
    if(USE_OPENCV)
        message(STATUS "Fetching OpenCV...")

        find_package(OpenCV QUIET)

        if(NOT OpenCV_FOUND)
            message(STATUS "OpenCV not found. Downloading...")

            FetchContent_Declare(
                opencv
                GIT_REPOSITORY https://github.com/opencv/opencv.git
                GIT_TAG 4.9.0
            )

            # Minimalny zestaw komponentów OpenCV do budowy
            set(BUILD_LIST "core,imgproc,highgui,video,videoio" CACHE STRING "" FORCE)
            set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
            set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
            set(BUILD_opencv_python_bindings_generator OFF CACHE BOOL "" FORCE)

            FetchContent_MakeAvailable(opencv)
        endif()
    else()
        message(STATUS "OpenCV is disabled")
    endif()

endfunction()
