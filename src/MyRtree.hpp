#ifndef SRC_MYRTREE_HPP
#define SRC_MYRTREE_HPP

#include <functional>
#include "./include/RTree.h"

class point2D {
public:
    point2D() : x(0), y(0) {
        std::cout<<"err.."<<endl;
    }


    point2D(float x, float y) : x(x), y(y) {
    }

    float x;
    float y;
};

class svgPoint {
public:
    svgPoint(){

    }
    svgPoint(point2D point, unsigned curve_id, unsigned point_id, unsigned id) : point(point), curve_id(curve_id),
                                                                                 point_id(point_id), id(id) {
    }

    friend ostream &operator<<(ostream &os, const svgPoint svgPoint) {
        os << " [" << svgPoint.point.x << " , " << svgPoint.point.y << "] "
           << svgPoint.curve_id << " "
           << svgPoint.point_id << " "
           << svgPoint.id << " "
           << endl;
        return os;
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

        float x = point.x;
        float y = point.y;

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

    RTree<svgPoint *, float, 2, float> rtree;
};


#endif //SRC_MYRTREE_HPP
