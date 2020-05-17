cmake_minimum_required(VERSION 3.0)
project(fastMapper)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#add_subdirectory(./src/include/)

add_compile_options("-D_hypot=hypot")

include_directories(./src)
include_directories(./src/include)

aux_source_directory(SOURCE_FILES ./src)
aux_source_directory(SOURCE_FILES_LIB ./src/include)

set(CPP_SRC_LIST ../src/data.hpp
        ../src/declare.hpp
        ../src/imageModel.hpp
#        ../src/MyRtree.hpp
#        ../src/svg.hpp
        ../src/unit.hpp
        ../src/wave.hpp
        ../src/wfc.hpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)

add_executable(fastMapper ${CPP_SRC_LIST} ../src/main.cpp)


#pybind11相关
#find_package(pybind11)
#pybind11_add_module(fastMapper_pybind ./src/fastMapper_pybind.cpp)
#pybind11相关结束

# lua导出接口
#include_directories("./src/lua/include/")
#file(GLOB LUA_SOURCE_FILES ./src/lua/include/*)
#add_library(fastmapper_dll SHARED ${PROJECT_SOURCE_DIR}/src/fastMapper_to_lua.cpp  ${LUA_SOURCE_FILES})
# lua导出接口 结束