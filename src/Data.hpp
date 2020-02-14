#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "Matrix.hpp"
#include "color.hpp"
#include "image.hpp"
#include "declare.hpp"
#include "direction.hpp"
#include "base.hpp"

#include "include/stb_image.h"
#include "include/stb_image_write.h"

using namespace std;

template<class T>
class Data : public Base {
public:

    Data(const Options &op) : options(op), Base(op) {}

    virtual void showResult(Matrix<unsigned> mat);

    //每个维度的尺寸大小
    //图像是2维，三维物体是3维
    std::vector<int> dims;

    std::vector<Matrix<Cell>> patterns;

    std::vector<double> patterns_frequency;

    Matrix<Cell> m;
    std::vector<std::array<std::vector<unsigned>, 4>> propagator;
    Matrix<Cell> _data;
    Options options;
};

#endif // SRC_DATA_HPP