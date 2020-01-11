cmake_minimum_required(VERSION 3.9)
project(fastwfc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})



include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
add_executable(single src/main.cpp ${SOURCE_FILES})

