#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "declare.hpp"
#include "MyRtree.hpp"

using namespace std;

template<typename T>
class Matrix;

template<class T, class AbstractFeature>
class Data {
public:
    Data(const Options &op):options(op) {}

    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "Data row func err res.." << std::endl;
    };

    const Options options;

    bool isVaildPatternId(unsigned pId) {
        unsigned y = pId / this->options.wave_width;
        unsigned x = pId % this->options.wave_width;

        if (x < 0 || x >= (int) this->options.wave_width) {
            return false;
        }
        if (y < 0 || y >= (int) this->options.wave_height) {
            return false;
        }
        return true;
    }


    DirectionSet _direction  = DirectionSet(8);

    std::vector<std::vector<svgPoint *>> data;      //原始的数据
    std::vector<std::vector<std::vector<unsigned>> > propagator;
    std::vector<AbstractFeature> feature;                             //图案数据
    std::vector<double> features_frequency;                            //图案频率

};

#endif // SRC_DATA_HPP