cmake_minimum_required(VERSION 3.0)
project(fastMapper)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_compile_options("-D_hypot=hypot")

include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
aux_source_directory(SOURCE_FILES_LIB ./src/include)

file(GLOB CPP_SRC_LIST ${CMAKE_CURRENT_LIST_DIR}/src/*.[c|h]pp)

set(CPP_SRC_LIST ../src/data.hpp
        ../src/declare.hpp
        ../src/imageModel.hpp
        ../src/MyRtree.hpp
        ../src/propagator.hpp
        ../src/svg.hpp
        ../src/unti.hpp
        ../src/wave.hpp
        ../src/wfc.hpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/python)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)

add_executable(main ${CPP_SRC_LIST} ../src/fastMapper.cpp)


#导入pybind包
find_package(pybind11)
#链接pybind文件
pybind11_add_module(fastMapper_pybind ./src/pybind/fastMapper_pybind.cpp)
