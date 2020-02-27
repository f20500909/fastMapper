#ifndef SRC_MYRTREE_HPP
#define SRC_MYRTREE_HPP

#include <functional>
#include "./include/RTree.h"


class svgPoint;

class point2D;

class MyRtree {
public:

    MyRtree() {

    }

    MyRtree(int count) {

    }

    std::vector<svgPoint *> getNearPoints(svgPoint *po) {
        std::vector<svgPoint *> res;

        point2D point = po->p;

        int x = point.x;
        int y = point.y;

        std::function<bool(svgPoint *)> func = [&](svgPoint *po) {
            res.push_back(po);
            return true;
        };

        rtree.Search(&x, &y, func);
        return res;
    }

    void insert(svgPoint *svgPoint) {
        point2D point = svgPoint->point;
        rtree.Insert(&point.x, &point.y, svgPoint);
    }

    RTree<svgPoint *, int, 2, float> rtree;
};


#endif //SRC_MYRTREE_HPP
