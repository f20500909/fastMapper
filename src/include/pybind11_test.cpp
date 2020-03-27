#include<pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(fastMapper, m) {

    m.doc() = "this is doc ...";

    // Add bindings here
    m.def("", []() {
        return "Hello, World!";
    });

}
