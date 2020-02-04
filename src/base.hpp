#ifndef SRC_BASE_HPP
#define SRC_BASE_HPP

#include <vector>
#include <string>
#include <algorithm>


using namespace std;

class Base {
public:
    Base(const Options &op) : option(op),_direction(op.directionSize) {
    }

    Direction _direction;
    const Options option;
    bool isVaildCoordinate(coordinate coor){
        int  x=coor[0];
        int  y=coor[0];
        if (x < 0 || x >= (int) option.wave_width) {
            return false;
        }
        if (y < 0 || y >= (int) option.wave_height) {
            return false;
        }
        return true;
    }

public:

};

#endif // SRC_DASE_HPP