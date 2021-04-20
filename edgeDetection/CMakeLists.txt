cmake_minimum_required(VERSION 3.19)
project(edgeDetection C)

set(CMAKE_C_STANDARD 99)

add_executable(edgeDetection main.c sobel.h)

find_package(OpenCL REQUIRED)
target_link_libraries(edgeDetection OpenCL::OpenCL)

