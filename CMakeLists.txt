cmake_minimum_required(VERSION 3.9)
project(fastMapper)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/cmdline.h)

add_executable(fastMapper ${SOURCE_FILES})

