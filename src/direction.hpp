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



    Direction()  {
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

    int operator+(Direction &d) {

    }


    std::vector<int> direct;
    std::vector<std::vector<int>> direct_2d = {{0,  -1},
                                             {-1, 0},
                                             {1,  0},
                                             {0,  0}};

    std::vector<int> directions_x = {0, -1, 1, 0};
    std::vector<int> directions_y = {-1, 0, 0, 1};

    static void doFunc(int start,int end,void* func){
//        for(int i= start ;i<end;i++){
//            func();
//        }


    }

};


#endif
