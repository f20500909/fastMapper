#this file is only for test !!
cmake_minimum_required(VERSION 3.0)
project(temp)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})


add_compile_options("-D_hypot=hypot")

include_directories(./)


#导入pybind包
find_package(pybind11)
#链接pybind文件
pybind11_add_module(fastMapper_pybind fastMapper_pybind.cpp)

