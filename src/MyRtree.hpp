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


    double get_angle(point2D& p1,point2D& p2)
    {


        double x1 = p1.x;
         double y1 = p1.y;

        double x2 = p2.x;
        double y2 = p2.y;

        double x3 = this->x;
        double y3 = this->y;


        double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
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

    //回调函数需要返回true
    std::vector<svgPoint *> getNearPoints(svgPoint *pSvgPoint,float distance) {
        std::vector<svgPoint *> res;
        point2D point = pSvgPoint->point;


        float min[2] ={point.x-distance,point.y-distance};
        float max[2] ={point.y+distance,point.y+distance};

        std::function<bool(svgPoint *)> func = [&](svgPoint *po) {
            res.push_back(po);
            return true;
        };

        rtree.Search(min, max, func);
        return res;
    }


    std::vector<svgPoint *> getSameBranchPoint(svgPoint *pSvgPoint) {
        return  this->rtree.getSameBranchData(pSvgPoint);
    }

    //knn
    std::vector<svgPoint *> knnSearch(svgPoint *pSvgPoint,int number) {
        std::vector<svgPoint *> res;
        point2D point = pSvgPoint->point;

        return res;
    }


    void insert(svgPoint *pSvgPoint) {
        point2D point = pSvgPoint->point;

        float min[2] ={point.x,point.y};
        float max[2] ={point.x +1,point.y +1};

        rtree.Insert(min, max, pSvgPoint);
    }

    RTree<svgPoint *, float, 2, float> rtree;
};


#endif //SRC_MYRTREE_HPP
