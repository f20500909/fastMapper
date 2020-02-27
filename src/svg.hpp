#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "data.hpp"
#include "declare.hpp"


class point2D {
public:
    point2D(int x, int y) : x(x), y(y) {
    }

    int x;
    int y;
};

class svgPoint {
public:

    svgPoint(point2D p, unsigned curve_id, unsigned point_id, unsigned id) : p(p), curve_id(curve_id), point_id(point_id),id(id) {
    }


    point2D p;
    unsigned curve_id;
    unsigned point_id;
    unsigned id;

};


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

            unsigned  id_1 = std::get<3>(this->data[i]);
            unsigned  id_2 = std::get<3>(fea.data[i]);
            if(id_1 != id_2){
                return false;
            }

        }
        return true;
    }

/**
 * Assign the matrix a to the current matrix.
 */
    SvgAbstractFeature &operator=(const SvgAbstractFeature & fea) noexcept {
        data = fea.data;
        neighborPatternId = fea.neighborPatternId;
        return *this;
    }

    /**
    * Check if two 2D arrays are equals.
    */
    bool operator==(const SvgAbstractFeature & fea) const noexcept {
//        if (data != fea.data){
//            return false;
//
//        };
//        if (neighborPatternId != fea.neighborPatternId){
//            return false;
//
//        };
        return true;
    }

    void getNeighborId(const std::vector<unsigned> neighborId) {
        this->neighborId = neighborId;
    }

    void neighborPatternId(const std::vector<unsigned> neighborId) {
        this->neighborId = neighborId;
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
        this->rtree.Insert(svgPoint); // Note, all values including zero are fine in this version
    }

    const point2D getPoint(svgPoint const &svgPoint) {
        return svgPoint.p;
    }

    unsigned getSvgPatternId(svgPoint &svgPoint) {
        return svgPoint.curve_id;
    }

    unsigned getSvgPointId(svgPoint &svgPoint) {
        return svgPoint.point_id;
    }

    unsigned getSvgUniqueId(svgPoint &svgPoint) {
        return svgPoint.id;
    }


    //插入点 其标识码为点的序列
    void insert(const point2D &p) {
        insert(p, count++);
    }

    void insert(const point2D &p, unsigned id) {
        svgPoint svgPoint(p, 1, 1, id);
        insert(svgPoint);
    }

    void setDistanceThreshold(int dis) {
        distanceThreshold = dis;
    }


//    //获取邻近点位
//    std::vector<svgPoint> getNearItem(point2D &p, int distance) {
//        std::vector<svgPoint> res;
//
//        auto _rule = [&](svgPoint &v) { return boost::geometry::distance(getPoint(v), p) < distance; };
//        rtree.query(boost::geometry::index::satisfies(_rule), std::back_inserter(res));
//        return res;
//    }


//    SvgAbstractFeature getSubFeature(unsigned id){
//
//    }


    SvgAbstractFeature getSubFeature(point2D po) {
        SvgAbstractFeature res;

        rtree.Search(, std::back_inserter(res.data));
        return res;
    }

    SvgAbstractFeature getSubFeature(point2D po, unsigned distanceThreshold) {
        SvgAbstractFeature res;

        this->rtree.Search(, std::back_inserter(res.data));
        return res;
    }


    int distanceThreshold;
    RTree<int, int, 2, float> rtree;
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
                symmetries[0] = spatialSvg.getSubFeature(data[i][j],40);
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
            // 应查询此点的
            std::vector<unsigned> tempPattern = this->patterns[pattern1].neighborPatternId;

            //对每个特征元素  的 每个邻居
            for (unsigned neighborId = 0;  neighborId < tempPattern.size();neighborId++) {

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
        std::vector<std::string> tmp = get_svg_data(this->options.input_data);
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
            std::vector<point2D> singlePolylinePoint;
            unsigned lenSum = 0;

            std::string &singlePolylineStr = strVector[i];
            boost::split(vecSegTag, singlePolylineStr, boost::is_any_of((" ,")));

            for (int j = 0; j < vecSegTag.size(); j = j + 2) {
                point2D tmp(static_cast<float>(atof(vecSegTag[j].c_str())),
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


    virtual void showResult(Matrix<unsigned> mat){
        std::cout<<"svg res ...."<<std::endl;
    };

private:
    std::vector<std::vector<point2D>> data;      //原始的数据
    std::vector<unsigned> len;        //累计的长度列表
    SpatialSvg spatialSvg;                  //封裝好的rtree  svg接口
    unsigned limit;                         //限制距离
};

#endif //SRC_SVG_HPP
