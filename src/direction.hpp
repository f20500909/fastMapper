#ifndef SRC_DIRECTION_HPP
#define SRC_DIRECTION_HPP

#include <vector>
#include <string>
#include <iostream>

#include "declare.hpp"

class Options;

class Direction {
public:

//    const Options option;



    Direction() {
    }

    Direction(const Options &op) : option(op) {

    }

    Direction(std::string type) {
        if (type == "x") {
//            directions_x

        }
    }

    const Options option;
    const int dim = 2;


    std::vector<int> direct;
    std::vector<std::vector<int>> direct_2d = {{0,  -1},
                                               {-1, 0},
                                               {1,  0},
                                               {0,  0}};

    std::vector<int> directions_x = {0, -1, 1, 0};
    std::vector<int> directions_y = {-1, 0, 0, 1};

    template<class Fun,class ...Ts>
    static void doEveryDirectId(Fun fun, Ts...agv) {
        for (int i = 0; i < totalSize; i++) {
            fun(i, std::forward<Ts>(agv)...);
        }
    }


    static int totalSize;
};

int Direction::totalSize = 4;

#endif
