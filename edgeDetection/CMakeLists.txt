cmake_minimum_required(VERSION 3.17)
project(edgeDetection C)

set(CMAKE_C_STANDARD 11)

add_executable(edgeDetection
        main.c
        sobel.h
		file_reader.h
		file_reader.c
		opencl_kernel_reader.h
		opencl_kernel_reader.c
		opencl_utils.c
		opencl_utils.h
		simple_timer.h
		simple_timer.c
		edgeDetection.h
		edgeDetection.c)

find_package(OpenCL REQUIRED)
target_link_libraries(edgeDetection OpenCL::OpenCL)

