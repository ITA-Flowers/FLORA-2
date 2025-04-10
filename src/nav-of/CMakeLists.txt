cmake_minimum_required(VERSION 3.10)
project(nav_of)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

# Zbieramy wszystkie .cpp z bieżącego katalogu
file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(nav_of_main ${SOURCES})

# ⬇️ USTAWIAMY include path NA PEWNO
target_include_directories(nav_of_main PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(nav_of_main ${OpenCV_LIBS})
