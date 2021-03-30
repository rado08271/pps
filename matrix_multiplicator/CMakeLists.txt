cmake_minimum_required(VERSION 3.7)
project(matrix_multiplicator C)

set(CMAKE_C_STANDARD 11)

add_executable(matrix_multiplicator main.c file_reader.c file_reader.h matrix_reader.h matrix_reader.c helper.c helper.h main.h matrix_computer.h matrix_computer.c opencl_kernel_reader.h opencl_kernel_reader.c opencl_utils.c opencl_utils.h ui.c ui.h simple_timer.c simple_timer.h)

find_package(OpenCL REQUIRED)
target_link_libraries(matrix_multiplicator OpenCL::OpenCL)
