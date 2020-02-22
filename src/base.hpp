#ifndef SRC_BASE_HPP
#define SRC_BASE_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>

#include "declare.hpp"
#include "position.hpp"

using namespace std;

class Base {
public:
    Base(const Options &op) : options(op), _direction(op.directionSize) {
    }

    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "err res.." << std::endl;
    };

    Position _direction;
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

public:

};

#endif // SRC_DASE_HPP