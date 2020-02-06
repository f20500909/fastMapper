cmake_minimum_required(VERSION 3.9)
project(demo_temp)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#add_subdirectory(test_temp)


set(OpenCV_DIR "D:\\package\\developer\\opencv\\wh_build\\install")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})


link_directories(./src/lib)
link_directories(./src/bin)


include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
add_executable(demo_temp src/main.cpp ${SOURCE_FILES})


target_link_libraries(demo_temp ${OpenCV_LIBS})
