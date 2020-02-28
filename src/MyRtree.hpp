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


    double get_angle(point2D& p1,point2D& p3)
    {

        float x1 =p1.x;
        float y1=p1.y;

        float x3=p3.x;
        float y3=p3.y;


        double theta = atan2(x1 - x3, y1 - y3) - atan2(this->x - x3, this->y - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }


    double get_angle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }

    double get_angle(double x1, double y1, double x3, double y3)
    {
        double theta = atan2(x1 - x3, y1 - y3) - atan2(x - x3, y - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
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
