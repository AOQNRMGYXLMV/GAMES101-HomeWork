cmake_minimum_required (VERSION 3.19.4)

project (learn-cg)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=1)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(homework0)
add_subdirectory(homework1)
add_subdirectory(homework2)
add_subdirectory(homework3)
add_subdirectory(homework4)
add_subdirectory(homework5)
add_subdirectory(homework6)
add_subdirectory(homework7)

