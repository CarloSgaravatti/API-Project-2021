cmake_minimum_required(VERSION 3.10)
project(api2021)

add_executable(api2021 api2021.c)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g3")