cmake_minimum_required (VERSION 3.19.4)

project (learn-cg)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_subdirectory(HW0)

