#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>
#include "Data.hpp"

typedef boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian> point2d;

//                   点       线条id     点id      身份id
//                            curve_id   point_id   id
typedef std::tuple<point2d, unsigned, unsigned, unsigned> svgPoint;

//尽量用唯一id来遍历
class SpatialSvg {
public:
    SpatialSvg() {
        setDistanceThreshold(5);
    }

    void insert(const svgPoint &svgPoint) {
        rtree.insert(svgPoint);
    }

    const point2d getPoint(svgPoint const &svgPoint) {
        return std::get<0>(svgPoint);
    }

    unsigned getSvgPatternId(svgPoint &svgPoint) {
        return std::get<1>(svgPoint);
    }

    unsigned getSvgPointId(svgPoint &svgPoint) {
        return std::get<2>(svgPoint);
    }

    unsigned getSvgUniqueId(svgPoint &svgPoint) {
        return std::get<3>(svgPoint);
    }


    //插入点 其标识码为点的序列
    void insert(const point2d &p) {
        insert(p, count++);
    }

    void insert(const point2d &p, unsigned id) {
        svgPoint svgPoint(p, 1, 1, id);
        insert(svgPoint);
    }

    void setDistanceThreshold(int dis) {
        distanceThreshold = dis;
    }

    std::vector<svgPoint> getVaildPoint() {
        std::vector<svgPoint> res;
        point2d sought = point2d(0, 0);

        auto _rule = [&](svgPoint const &v) {
            return boost::geometry::distance(getPoint(v), sought) < distanceThreshold;
        };

        rtree.query(boost::geometry::index::satisfies(_rule), std::back_inserter(res));
        return res;
    }

//    //获取邻近点位
//    std::vector<svgPoint> getNearItem(point2d &p, int distance) {
//        std::vector<svgPoint> res;
//
//        auto _rule = [&](svgPoint &v) { return boost::geometry::distance(getPoint(v), p) < distance; };
//
//        rtree.query(boost::geometry::index::satisfies(_rule), std::back_inserter(res));
//        return res;
//    }

    std::vector<svgPoint> getPattern(point2d &p) {
    }


    int getCount() {
        return count;
    }

    int distanceThreshold;
    int count;
    boost::geometry::index::rtree<svgPoint, boost::geometry::index::quadratic<32> > rtree;
};


template<class T>
class Svg : public Data<T> {
public:

    Svg(const Options op) : Data<T>(op) {
        parseData();
        initPatterns();
        generateCompatible();
    }

    void initPatterns() {
        // 将图案插入到rtree中
        unsigned curve_id = 0;
        for (auto iter = m.cbegin(); iter != m.cend(); iter++, curve_id++) {
            for (unsigned point_id = 0; point_id < iter->second.size(); point_id++) {
                svgPoint tmp(iter->second[curve_id], curve_id, point_id, iter->first);
                spatialSvg.insert(tmp);
            }
        }

    }

    void generateCompatible() {

    }

    void parseData() {
        auto tmp = get_svg_data(this->options.input_data);
        parseDataMap(tmp);
    }

    void parseDataMap(vector<string> &strVector) {
        //截取到有效片段
        for (int i = 0; i < strVector.size(); i++) {
            string row = strVector[i];
            int len = row.size();
            strVector[i] = row.substr(8, len - 8 - 2);
        }

        //将有效片段分割
        for (int i = 0; i < strVector.size(); i++) {
            std::vector<std::string> vecSegTag;
            vector<point2d> singlePolylinePoint;

            std::string &singlePolylineStr = strVector[i];
            boost::split(vecSegTag, singlePolylineStr, boost::is_any_of((" ,")));

            for (int j = 0; j < vecSegTag.size(); j = j + 2) {
                point2d tmp(static_cast<float>(atof(vecSegTag[j].c_str())),
                            static_cast<float>(atof(vecSegTag[j + 1].c_str())));
                singlePolylinePoint.push_back(tmp);
            }
            m[i] = singlePolylinePoint;
        }
    }

    vector<string> get_svg_data(string input_data) {
        string str = "(points)=\"[\\s\\S]*?\"";

        fstream in(input_data);
        string svg_context((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

        regex reg(str);
        smatch sm;
        vector<string> res;

        while (regex_search(svg_context, sm, reg)) {
            res.push_back(sm.str());
            svg_context = sm.suffix();
        }
        return res;
    }

private:
    std::unordered_map<int, vector<point2d>> m;
    SpatialSvg spatialSvg;
};

#endif //SRC_SVG_HPP
