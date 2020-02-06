cmake_minimum_required(VERSION 3.9)
project(fastwfc)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_subdirectory(test_temp)


link_directories(./src/lib)
link_directories(./src/bin)


include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
add_executable(single src/main.cpp ${SOURCE_FILES})


#target_link_libraries(single cairo-static)
