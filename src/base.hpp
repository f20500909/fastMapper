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
class CoordinateState;

class Base {
public:
    Base(const Options &op) : options(op){
    }

    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "err res.." << std::endl;
    };

    const Options options;

    bool isVaildCoordinate(CoordinateState coor) {
        if (coor.x < 0 || coor.x >= (int) this->options.wave_width) {
            return false;
        }
        if (coor.y < 0 || coor.y >= (int) this->options.wave_height) {
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