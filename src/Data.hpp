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

using namespace std;

template<class T>
class Data : public Base {
public:

    Data(const Options &op):Base(op) {}

    virtual void showResult(Matrix<unsigned> mat){
        std::cout<<"Data row func err res.."<<std::endl;
    };


    //每个维度的尺寸大小
    //图像是2维，三维物体是3维
    std::vector<Matrix<Cell>> patterns;
    std::vector<double> patterns_frequency;
    std::vector<std::array<std::vector<unsigned>, 4>> propagator;

    // 原始从输入文件读取得到的数据
    Matrix<Cell> _data;
};

#endif // SRC_DATA_HPP