#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

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

    std::vector<Matrix<Cell>> patterns;
    std::vector<double> patterns_frequency;
    std::vector<std::array<std::vector<unsigned>, 4>> propagator;


};

#endif // SRC_DATA_HPP