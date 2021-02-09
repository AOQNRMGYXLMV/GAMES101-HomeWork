cmake_minimum_required (VERSION 3.19.4)

project (learn-cg)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=1)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_subdirectory(homework0)
add_subdirectory(homework1)

