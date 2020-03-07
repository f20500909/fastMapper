cmake_minimum_required(VERSION 3.0)
project(fastMapper)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})



include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
aux_source_directory(SOURCE_FILES_LIB  ./src/include)

file(GLOB CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/src/*.[c|h]pp)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/output)

add_executable(main  ${CPP_SRC_LIST})
