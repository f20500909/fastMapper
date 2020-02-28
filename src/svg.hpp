#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "data.hpp"
#include "MyRtree.hpp"

class MyRtree;


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


/**
 * Assign the matrix a to the current matrix.
 */
    SvgAbstractFeature &operator=(const SvgAbstractFeature &fea) noexcept {
        this->data = fea.data;
        this->basePoint = fea.basePoint;
        return *this;
    }

    std::vector<svgPoint *> data;
    std::vector<unsigned> neighborIds;
    svgPoint basePoint;
};

//TODO 完成hash函数
namespace std {
    template<>
    class hash<SvgAbstractFeature> {
    public:
        size_t operator()(const SvgAbstractFeature &fea) const {
            std::size_t seed = fea.data.size();
            for (int i = 0; i < fea.data.size(); i++) {
                seed ^= std::size_t(fea.data[i]->point.x) + (seed << 6) + (seed >> 2);
                seed ^= std::size_t(fea.data[i]->point.y) + (seed << 6) + (seed >> 2);
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

    void insert(svgPoint *svgPoint) {
        this->rtree.insert(svgPoint); // Note, all values including zero are fine in this version
    }

    const point2D getPoint(svgPoint *svgPoint) {
        return svgPoint->point;
    }

    unsigned getSvgPatternId(svgPoint *svgPoint) {
        return svgPoint->curve_id;
    }

    unsigned getSvgPointId(svgPoint *svgPoint) {
        return svgPoint->point_id;
    }

    unsigned getSvgUniqueId(svgPoint *svgPoint) {
        return svgPoint->id;
    }

    void setDistanceThreshold(int dis) {
        distanceThreshold = dis;
    }

    SvgAbstractFeature getSubFeature(svgPoint *point, int distance) {
        SvgAbstractFeature res;
        res.data = this->rtree.getNearPoints(point, distance);
        for (int i =0 ;i<res.data.size();i++){

            if (point->id!=res.data[i]->id) {
                res.neighborIds.push_back(res.data[i]->id);
            }
        }
        res.basePoint = *point;
        return res;
    }

    int distanceThreshold;
    MyRtree rtree;
};


bool operator==(SvgAbstractFeature left, SvgAbstractFeature right) {
    for (unsigned i = 0; i < left.data.size(); i++) {
        if (left.data[i]->point.x != right.data[i]->point.x) {
            return false;
        }
        if (left.data[i]->point.y != right.data[i]->point.y) {
            return false;
        }
    }
    return true;
}

template<class T, class AbstractFeature>
class data;

class Options;

template<class T, class AbstractFeature>
class Svg : public Data<T, AbstractFeature> {
public:

    Svg(const Options &op) : Data<T, AbstractFeature>(op) {
        parseData();
        initPatterns();
        generateCompatible();
    }

    void initPatterns() {
        // 将图案插入到rtree中
        unsigned id = 0;
        for (int i = 0; i < data.size(); i++) {
            for (unsigned j = 0; j < data[i].size(); j++) {
                spatialSvg.insert(data[i][j]);
            }
        }

        // 获取一个点临近的点位，点位附近的最近点做为特征图案
        std::unordered_map<SvgAbstractFeature, unsigned> patterns_id;
        std::vector<SvgAbstractFeature> symmetries(this->options.symmetry);

        for (int i = 0; i < data.size(); i++) {
            for (unsigned j = 0; j < data[i].size(); j++) {
                symmetries[0] = spatialSvg.getSubFeature(data[i][j], 40);
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
            std::vector<unsigned> neighborIds = this->patterns[pattern1].neighborIds;

            //对每个特征元素  的 每个邻居
            for (unsigned neighborId = 0; neighborId < neighborIds.size(); neighborId++) {

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

        unsigned cnt = 0;
        //将有效片段分割
        for (int i = 0; i < strVector.size(); i++) {
            std::vector<std::string> vecSegTag;
            std::vector<svgPoint *> singlePolylinePoint;
            unsigned lenSum = 0;

            std::string &singlePolylineStr = strVector[i];
            boost::split(vecSegTag, singlePolylineStr, boost::is_any_of((" ,")));

            for (int j = 0; j < vecSegTag.size(); j = j + 2) {
                point2D tempPoint2D(static_cast<float>(atof(vecSegTag[j].c_str())),
                                    static_cast<float>(atof(vecSegTag[j + 1].c_str())));
                svgPoint *tempSvgPoint = new svgPoint(tempPoint2D, i, j, cnt++);
                singlePolylinePoint.push_back(tempSvgPoint);
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


    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "svg res ...." << std::endl;
    };

private:
    std::vector<std::vector<svgPoint *>> data;      //原始的数据
    std::vector<unsigned> len;        //累计的长度列表
    SpatialSvg spatialSvg;                  //封裝好的rtree  svg接口
    unsigned limit;                         //限制距离
};

#endif //SRC_SVG_HPP
