cmake_minimum_required(VERSION 3.9)
project(fastMapper)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#add_subdirectory(test_temp)

set(OpenCV_DIR "D:\\package\\developer\\opencv\\wh_build\\install")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})


set(BOOST_ROOT "D:\\package\\developer\\boost_1_69_0\\boost")
find_package(Boost)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
aux_source_directory(SOURCE_FILES_LIB  ./src/include)

file(GLOB CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/src/*.[c|h]pp)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/output)
#SET(LIB_SRC ${SOURCE_FILES} ${CPP_SRC_LIST})

add_executable(main  ${CPP_SRC_LIST})
#ADD_LIBRARY(fastMapperLIb SHARED  ${CPP_SRC_LIST})

#link_directories(./output)
#add_executable(runExe src/run.cpp)
#TARGET_LINK_LIBRARIES(runExe libfastMapperLIb.dll)
