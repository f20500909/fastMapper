#ifndef SRC_MYRTREE_HPP
#define SRC_MYRTREE_HPP

#include <functional>
#include "./include/RTree.h"
#include "svg.hpp"

class point2D;

class svgPoint;


class Mytree {
public:

    Mytree() {

    }

    Mytree(int count) {

    }

    std::vector<unsigned> getNearPoints(const svgPoint &p) {
        std::vector<unsigned> res;
        const int x = p.p.x;
        const int y = p.p.y;

        auto func = [&](int id) { res.push_back(id); return true; };

        tree.Search(&x, &y, func);
        return res;
    }



    void insert(const svgPoint &p) {

    }

    RTree<int, int, 2, float> tree;
};


#endif //SRC_MYRTREE_HPP
