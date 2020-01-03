cmake_minimum_required(VERSION 3.9)
project(fastwfc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(DEFAULT_BUILD_TYPE "Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})


if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif ()

include(GNUInstallDirs)
include_directories(./src)

aux_source_directory(SOURCE_FILES ./src)


add_executable(wfc_demo src/main.cpp)
add_executable(single src/single.cpp ${SOURCE_FILES})

