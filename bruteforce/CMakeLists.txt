cmake_minimum_required(VERSION 3.17)
project(brute C)
set(CMAKE_C_STANDARD 11)

add_executable(brute main.c main.h brute_force.c brute_force.h file_reader.c file_reader.h opencl_kernel_reader.c opencl_kernel_reader.h opencl_utils.c opencl_utils.h simple_timer.c simple_timer.h ui.c ui.h)

#target_link_libraries(brute m)
find_package(OpenCL REQUIRED)
target_link_libraries(brute OpenCL::OpenCL)