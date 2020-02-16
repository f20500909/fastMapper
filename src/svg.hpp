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
        unsigned id = 0;
        for (int i = 0; i < _data.size(); i++) {
            for (unsigned j = 0; j < _data[i].size(); j++) {
                svgPoint tmp(_data[i][j], i, j, id++);
                spatialSvg.insert(tmp);
            }
        }

        // 获取一个点临近的点位，点位附近的最近点做为特征图案
        std::unordered_map<Matrix<unsigned>, unsigned> patterns_id;
        std::vector<Matrix<unsigned>> symmetries(this->options.symmetry, Matrix<unsigned>(this->options.N, this->options.N));
        unsigned max_i = this->_data.height - this->options.N + 1;
        unsigned max_j = this->_data.width - this->options.N + 1;


        for (unsigned i = 0; i < max_i; i++) {
            for (unsigned j = 0; j < max_j; j++) {
                symmetries[0].data = this->_data.get_sub_array(i, j, this->options.N, this->options.N).data;
                symmetries[1].data = symmetries[0].reflected().data;
                symmetries[2].data = symmetries[0].rotated().data;
                symmetries[3].data = symmetries[2].reflected().data;
                symmetries[4].data = symmetries[2].rotated().data;
                symmetries[5].data = symmetries[4].reflected().data;
                symmetries[6].data = symmetries[4].rotated().data;
                symmetries[7].data = symmetries[6].reflected().data;

                for (unsigned k = 0; k < this->options.symmetry; k++) {
                    auto res = patterns_id.insert(std::make_pair(symmetries[k], this->patterns.size()));
                    if (!res.second) {
                        this->patterns_frequency[res.first->second] += 1;
                    } else {
                        this->patterns.push_back(symmetries[k]);
                        this->patterns_frequency.push_back(1);
                    }
                }
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
            _data.push_back(singlePolylinePoint);
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
    std::vector<vector<point2d>> _data; //原始的数据
    SpatialSvg spatialSvg;
};

#endif //SRC_SVG_HPP
