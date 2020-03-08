#ifndef SRC_BASE_HPP
#define SRC_BASE_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>

#include "declare.hpp"

using namespace std;
class Options;
class DirectionSet;

class Base {
public:
    Base(const Options &op) : options(op){
    }

    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "err res.." << std::endl;
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

    DirectionSet _direction;

    std::vector<std::vector<std::vector<unsigned>> > propagator;
    std::vector<AbstractFeature> patterns;                             //图案数据
    std::vector<double> patterns_frequency;                            //图案频率
public:

};

#endif // SRC_DASE_HPP