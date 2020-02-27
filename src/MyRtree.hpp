#ifndef SRC_MYRTREE_HPP
#define SRC_MYRTREE_HPP

#include <functional>
#include "./include/RTree.h"

class point2D {
public:
    point2D() : x(0), y(0) {
        assert(false);
    }

    point2D(int x, int y) : x(x), y(y) {
    }

    int x;
    int y;
};

class svgPoint {
public:
    svgPoint(){

    }
    svgPoint(point2D point, unsigned curve_id, unsigned point_id, unsigned id) : point(point), curve_id(curve_id),
                                                                                 point_id(point_id), id(id) {
    }


    point2D point;
    unsigned curve_id;
    unsigned point_id;
    unsigned id;
};


class MyRtree {
public:

    MyRtree() {

    }

    MyRtree(int count) {

    }

    std::vector<svgPoint *> getNearPoints(svgPoint *pSvgPoint,int distance) {
        std::vector<svgPoint *> res;
        point2D point = pSvgPoint->point;

        int x = point.x;
        int y = point.y;

        std::function<bool(svgPoint *)> func = [&](svgPoint *po) {
            res.push_back(po);
            return true;
        };

        rtree.Search(&x, &y, func);
        return res;
    }

    void insert(svgPoint *pSvgPoint) {
        point2D point = pSvgPoint->point;
        rtree.Insert(&point.x, &point.y, pSvgPoint);
    }

    RTree<svgPoint *, int, 2, float> rtree;
};


#endif //SRC_MYRTREE_HPP
