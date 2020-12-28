cmake_minimum_required(VERSION 3.16)
project(istar_c C)

set(CMAKE_C_STANDARD 99)

add_executable(istar_c main.c)
target_link_libraries(istar_c curl)
