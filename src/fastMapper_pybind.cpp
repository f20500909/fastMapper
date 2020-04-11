#include<pybind11/pybind11.h>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <ctime>

#include "fastMapper.hpp"

using namespace std;
namespace py = pybind11;


//注意 pypi打包时  此处的模块名称必须与包名称一致 不然打包完成 pip 安装时编译会报错
PYBIND11_MODULE(fastMapper_pybind, m) {

    m.doc() = "this is doc ...";

    // Add bindings here
    m.def("foo", []() {
        return "Hello, World!";
    });


    m.def("run",
          [](unsigned out_height,
             unsigned out_width,
             unsigned symmetry,
             unsigned N,
             int channels,
             int log,
             string input_data,
             string output_data,
             string type) {
              single_run(out_height, out_width, symmetry, N, channels, log, input_data, output_data, type);
              return "done";
          });


}
