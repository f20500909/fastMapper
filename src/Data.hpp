#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "base.hpp"
//#include "svg_hpp"

using namespace std;

template<class T,class AbstractFeature>
class Data : public Base {
public:

    Data(const Options &op):Base(op) {}

    virtual void showResult(Matrix<unsigned> mat){
        std::cout<<"Data row func err res.."<<std::endl;
    };

    std::vector<AbstractFeature> patterns;
//    std::vector<AbstractFeature> patterns;
    std::vector<double> patterns_frequency;
    std::vector<std::array<std::vector<unsigned>, directionNumbers>> propagator;
};

#endif // SRC_DATA_HPP