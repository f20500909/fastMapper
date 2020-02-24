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
#include "data.hpp"
#include "declare.hpp"

typedef boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian> point2d;

//                   点       线条id     点id      身份id
//                            curve_id   point_id   id
typedef std::tuple<point2d, unsigned, unsigned, unsigned> svgPoint;


//特征单元 波函数塌陷的最小计算单元
class SvgAbstractFeature {
public:
    SvgAbstractFeature() {
    }

    //得到镜像图形
    SvgAbstractFeature reflected() {
        return *this;
    }

    //得到旋转后的图形
    SvgAbstractFeature rotated() {
        return *this;
    }
    bool operator == (const SvgAbstractFeature& fea){
        for(int i=0;i<data.size();i++){

            point2d  pointA = std::get<0>(this->data[i]);
            point2d  pointB = std::get<0>(fea.data[i]);
            if(pointA.get<0>()!=pointA.get<0>()){
                return false;
            }

            if(pointA.get<1>()!=pointA.get<1>()){
                return false;
            }

        }
        return true;
    }

    void setNeighborId(const std::vector<unsigned> neighborId) {
//        this->neighborId = neighborId;
    }

    std::vector<svgPoint> data;

    std::vector<unsigned> neighborPatternId;
};

//TODO 完成hash函数
namespace std {
    template<>
    class hash<SvgAbstractFeature> {
    public:
        size_t operator()(const SvgAbstractFeature &fea) const {
            std::size_t seed = fea.data.size();
            for (int i = 0; i < fea.data.size(); i++) {
                seed ^= std::size_t(std::get<3>(fea.data[i])) + (seed << 6) + (seed >> 2);
            };
            return seed;
        }
    };
};

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
//        rtree.query(boost::geometry::index::satisfies(_rule), std::back_inserter(res));
//        return res;
//    }


//    SvgAbstractFeature getSubFeature(unsigned id){
//
//    }


    SvgAbstractFeature getSubFeature(point2d po) {
        SvgAbstractFeature res;
        auto _rule = [&](svgPoint const &v) {
            return boost::geometry::distance(getPoint(v), po) < distanceThreshold;
        };
        rtree.query(boost::geometry::index::satisfies(_rule), std::back_inserter(res.data));
        return res;
    }


    int getCount() {
        return count;
    }

    int distanceThreshold;
    int count;
    boost::geometry::index::rtree<svgPoint, boost::geometry::index::quadratic<32> > rtree;
};

template<class T,class AbstractFeature>
class data ;

class Options;

template<class T,class AbstractFeature>
class Svg : public Data<T,AbstractFeature> {
public:

    Svg(const Options& op) : Data<T,AbstractFeature>(op) {
        parseData();
        initPatterns();
        generateCompatible();
    }

    void initPatterns() {
        // 将图案插入到rtree中
        unsigned id = 0;
        for (int i = 0; i < data.size(); i++) {
            for (unsigned j = 0; j < data[i].size(); j++) {
                svgPoint tmp(data[i][j], i, j, id++);
                spatialSvg.insert(tmp);
            }
        }

        // 获取一个点临近的点位，点位附近的最近点做为特征图案
        std::unordered_map<SvgAbstractFeature, unsigned> patterns_id;
        std::vector<SvgAbstractFeature> symmetries(this->options.symmetry);

        for (int i = 0; i < data.size(); i++) {
            for (unsigned j = 0; j < data[i].size(); j++) {
                symmetries[0] = spatialSvg.getSubFeature(data[i][j]);
                symmetries[1] = symmetries[0].reflected();
                symmetries[2] = symmetries[0].rotated();
                symmetries[3] = symmetries[2].reflected();
                symmetries[4] = symmetries[2].rotated();
                symmetries[5] = symmetries[4].reflected();
                symmetries[6] = symmetries[4].rotated();
                symmetries[7] = symmetries[6].reflected();

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

    void generateCompatible() noexcept {
        this->propagator = std::vector<std::array<std::vector<unsigned>, 4> >(this->patterns.size());

        //对每个特征元素
        for (unsigned pattern1 = 0; pattern1 < this->patterns.size(); pattern1++) {
            std::vector<unsigned> tempPattern = this->patterns[pattern1].neighborPatternId;

            //对每个特征元素  的 每个邻居
            for (unsigned neighborId = 0; neighborId++; neighborId < tempPattern.size()) {

                //对每个特征元素  的 每个邻居  的每个特征元素
                for (unsigned pattern2 = 0; pattern2 < this->patterns.size(); pattern2++) {
                    //此处重载了==号操作符
                    if (this->patterns[pattern1] == this->patterns[pattern2]) {
                        // 对每一个特征元素，其每一个方向，如果其相等 就把id存下
                        this->propagator[pattern1][neighborId].push_back(pattern2);
                    }
                }

            }
        }
    }

    void parseData() {
        auto tmp = get_svg_data(this->options.input_data);
        parseDataMap(tmp);
    }

    void parseDataMap(std::vector<std::string> &strVector) {
        //截取到有效片段
        for (int i = 0; i < strVector.size(); i++) {
            std::string row = strVector[i];
            int len = row.size();
            strVector[i] = row.substr(8, len - 8 - 2);
        }

        //将有效片段分割
        for (int i = 0; i < strVector.size(); i++) {
            std::vector<std::string> vecSegTag;
            std::vector<point2d> singlePolylinePoint;
            unsigned lenSum = 0;

            std::string &singlePolylineStr = strVector[i];
            boost::split(vecSegTag, singlePolylineStr, boost::is_any_of((" ,")));

            for (int j = 0; j < vecSegTag.size(); j = j + 2) {
                point2d tmp(static_cast<float>(atof(vecSegTag[j].c_str())),
                            static_cast<float>(atof(vecSegTag[j + 1].c_str())));
                singlePolylinePoint.push_back(tmp);
            }
            lenSum += singlePolylinePoint.size();
            len.push_back(lenSum);
            data.push_back(singlePolylinePoint);
        }
    }

    std::vector<std::string> get_svg_data(std::string input_data) {
        std::string str = "(points)=\"[\\s\\S]*?\"";

        std::fstream in(input_data);
        std::string svg_context((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        std::regex reg(str);
        std::smatch sm;
        std::vector<std::string> res;

        while (regex_search(svg_context, sm, reg)) {
            res.push_back(sm.str());
            svg_context = sm.suffix();
        }
        return res;
    }

private:
    std::vector<std::vector<point2d>> data;      //原始的数据
    std::vector<unsigned> len;        //累计的长度列表
    SpatialSvg spatialSvg;                  //封裝好的rtree  svg接口
    unsigned limit;                         //限制距离
};

#endif //SRC_SVG_HPP
