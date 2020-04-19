cmake_minimum_required(VERSION 3.0)
project(test)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})


include_directories(./)
include_directories(../)

add_executable(rtree_test  test_rtree.cpp)
add_executable(test_bitmap  test_bitmap.cpp)
