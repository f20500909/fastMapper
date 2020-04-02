#ifndef SRC_MYRTREE_HPP
#define SRC_MYRTREE_HPP

#include <functional>
#include "include/rtree.hpp"

#define M_PI float(3.14159265358979323846)
using namespace std;
using namespace rtree;

class point2D {
public:
    point2D() : x(0), y(0) {
//        std::cout << "err.." << endl;
    }

    point2D(float x, float y) : x(x), y(y) {
    }


    float get_angle(point2D &p1, point2D &p2) {
        float x1 = p1.x;
        float y1 = p1.y;

        float x2 = p2.x;
        float y2 = p2.y;

        float x3 = this->x;
        float y3 = this->y;

        float theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }


    float get_azimuth(point2D &p2) const {

        float x1 = this->x;
        float y1 = this->y + 1000;

        float x2 = p2.x;
        float y2 = p2.y;

        float x3 = this->x;
        float y3 = this->y;

        float theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }

    float get_distance(point2D &p) {
        float x2 = p.x;
        float y2 = p.y;
        return sqrt((this->x - x2) * (this->x - x2) + (this->y - y2) * (this->y - y2));
    }


    float get_angle(float x1, float y1, float x2, float y2, float x3, float y3) {
        float theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }

    float get_angle(float x1, float y1, float x3, float y3) {
        float theta = atan2(x1 - x3, y1 - y3) - atan2(x - x3, y - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }

    void shiftFormPoint(float dx, float dy) {
        x += dx;
        y += dy;
    }

    //得到点的偏移量 返回的是一个包含偏移信息的坐标
    point2D getPointShift(point2D &p) {
        point2D shift;
        shift.x = this->x - p.x;
        shift.y = this->y - p.y;
        return shift;
    }

    void shiftFormPoint(point2D &p) {
        this->x += p.x;
        this->y += p.y;

    }


    float x;
    float y;
};

class svgPoint {
public:
    svgPoint() {

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
//    std::vector<svgPoint *> getNearPoints(svgPoint *pSvgPoint, float distance) {
//        std::vector<svgPoint *> res;
//        point2D point = pSvgPoint->point;
//
//
//        float min[2] = {point.x - distance, point.y - distance};
//        float max[2] = {point.y + distance, point.y + distance};
//
//        std::function<bool(svgPoint *)> func = [&](svgPoint *po) {
//            res.push_back(po);
//            return true;
//        };
//
//        rtree.Search(min, max, func);
//        return res;
//    }


    std::vector<svgPoint *> get_nearest(svgPoint *pSvgPoint) {
        std::vector<svgPoint *> temp;
        std::vector<svgPoint *> res;
        rtree.nearest_search(Point(0, 0), 8 + 1, temp);
        for (int i = 0; i < temp.size(); i++) {
            if(pSvgPoint->id!=temp[i]->id){
                res.push_back(temp[i]);
            }
        }
        return res;
    }

    //knn
    std::vector<svgPoint *> knnSearch(svgPoint *pSvgPoint, int number) {
        std::vector<svgPoint *> res;
        point2D point = pSvgPoint->point;

        return res;
    }


    void insert(svgPoint *pSvgPoint) {
        point2D point = pSvgPoint->point;

        rtree.insert(Rect(point.x, point.y, point.x + 1, point.y + 1), pSvgPoint);

    }

//    RTree<svgPoint *, float, 2, float> rtree;
//    RTree<svgPoint *, float, 2, float> rtree;
    R_tree<svgPoint *> rtree;

};

#endif //SRC_MYRTREE_HPP
