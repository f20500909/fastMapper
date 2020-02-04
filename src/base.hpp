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

public:

};

#endif // SRC_DASE_HPP