#include<pybind11/pybind11.h>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <ctime>

#include "include/cmdline.h"
#include "imageModel.hpp"

using namespace std;
namespace py = pybind11;


void single_run(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels, string input_data,
                string output_data, string type) {
    srand((unsigned) time(NULL));

//    input_data = "../samples/ai/wh1.svg";
//    type = "svg";

    const Config options = {out_height, out_width, symmetry, N, channels, input_data, output_data, type};

//    Data<int, AbstractFeature> *data = new Svg<int, AbstractFeature>(options);
    Data<int, AbstractFeature> *data = new Img<int, AbstractFeature>(options);

    WFC wfc(data);
    wfc.run();
    delete data;
}


//注意 pypi打包时  此处的模块名称必须与包名称一致 不然打包完成 pip 安装时编译会报错
PYBIND11_MODULE(fastMapper_pybind, m) {

    m.doc() = "this is doc ...";

    // Add bindings here
    m.def("foo", []() {
        return "Hello, World!";
    });


    m.def("run",
          [](unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels, string input_data,
             string output_data, string type) {

              single_run(out_height, out_width, symmetry, N, channels, input_data,
                         output_data, type);
              return "done";
          });


}
