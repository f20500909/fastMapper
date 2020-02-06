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

    int operator+(Direction &d) {

    }


    std::vector<int> direct;
    std::vector<std::vector<int>> direct_2d = {{0,  -1},
                                               {-1, 0},
                                               {1,  0},
                                               {0,  0}};

    std::vector<int> directions_x = {0, -1, 1, 0};
    std::vector<int> directions_y = {-1, 0, 0, 1};

//    template<typename T>
//    void printarg(T t) {
//        std::cout << t;
//    }
//
//    template<typename ...Ts>
//    void go(Ts...agv) {
//        gogo(std::forward<Ts>(agv)...);  // go->gogo 可变参数转发需要...
//
//    }
//
//    template<typename ...Ts>
//    void gogo(Ts...agvs) {
//        int arr[] = {(printarg(agvs), 0)...}; //->printarg返回值赋给0，0=返回值，最后值再进入数组;
//    }


    template<typename ...Ts>
    static void doEveryDirectId(std::function<void(int, int, int)> Func, Ts...agv) {
        for (int i = 0; i < totalSize; i++) {
            Func(i, std::forward<Ts>(agv)...);
        }
    }


    template<typename ...Ts>
    static void doEveryDirectId(std::function<void(int, int)> Func, Ts...agv) {
        for (int i = 0; i < totalSize; i++) {
            Func(i, std::forward<Ts>(agv)...);
        }
    }


    static int totalSize;
};

int Direction::totalSize = 4;

#endif
