#ifndef SRC_DIRECTION_HPP
#define SRC_DIRECTION_HPP

#include <vector>
#include <string>
#include <iostream>

#include "declare.hpp"

class Options;

class Position {
public:

    Position() {
    }

    Position(const int _positionSize) {

    }

    const int dim = 2;

    std::vector<Direction> _data = {{0,  -1},
                                {-1, 0},
                                {1,  0},
                                {0,  1}};

    Direction getPoint(unsigned directionId) {
        assert(directionId < _data.size());
        return _data[directionId];
    }

    template<class Fun, class ...Ts>
    void doEveryDirectId(Fun fun, Ts...agv) {
        for (int i = 0; i < _data.size(); i++) {
            fun(i, std::forward<Ts>(agv)...);
        }
    }

    template<class Fun, class ...Ts>
    void doEveryDirectData(Fun fun, Ts...agv) {
        for (int i = 0; i < _data.size(); i++) {
            fun(_data[i], std::forward<Ts>(agv)...);
        }
    }


};


#endif
