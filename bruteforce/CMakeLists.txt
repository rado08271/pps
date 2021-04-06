cmake_minimum_required(VERSION 3.17)
project(brute C)
set(CMAKE_C_STANDARD 11)

add_executable(brute main.c)

target_link_libraries(brute m)