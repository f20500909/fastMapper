#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "data.hpp"
#include "MyRtree.hpp"
#include "unti.hpp"
#include "./include/bitMap.h"

class MyRtree;


//特征单元 波函数塌陷的最小计算单元
class SvgAbstractFeature {
public:

    SvgAbstractFeature() {

    }

    SvgAbstractFeature(std::vector<svgPoint *> nearPoints, svgPoint basePoint, std::vector<std::vector<svgPoint *>> &allSvgData)
            : data(nearPoints), basePoint(basePoint) {
        for (int i = 0; i < this->data.size(); i++) {
            if (this->basePoint.id != this->data[i]->id) {
                this->neighborIds.push_back(this->data[i]->id);
            }
        }

        this->reSetVal(allSvgData);
    }

    //得到镜像图形
    SvgAbstractFeature reflected() {
        return *this;
    }

    //得到旋转后的图形
    SvgAbstractFeature rotated() {
        return *this;
    }

    //得到旋转后的图形


    /*
     * 位图法标识
     0000 0000   1 2 位 起始点  终止点
     0000 0000   同线段上的临近点数量
     0000 0000   不同线段上的临近点数量
     0000 0000   角度
     */
    void reSetVal(std::vector<std::vector<svgPoint *>> &data) {
        val = BitMap(64);
        //起始点判定
        if (basePoint.point_id == 0) {
            val.set(0, 1);
        }
        //终止点判定
        for (int i = 0; i < data.size(); i++) {
            int len = data[i].size();
            if (basePoint.point_id == data[i][len - 1]->point_id) {
                val.set(1, 1);
            }
        }

        //同线段上的临近点数量
        int sameCurvePointNumber = 0;
        for (int i = 0; i < this->data.size(); i++) {
            if (this->data[i]->curve_id == basePoint.curve_id) {
                sameCurvePointNumber++;
            }
        }

        sameCurvePointNumber = uint8_t(sameCurvePointNumber);
        sameCurvePointNumber = min(sameCurvePointNumber, 0xff);

        val.setNumber(1, sameCurvePointNumber);

        //不同线段上的临近点数量
        val.set(2, data.size() - sameCurvePointNumber);

        //角度掩码
        //如果是起点或者终点，角度默认设为0
        //既不是起点 也不是终点
        if (!val.get(0) and !val.get(1)) {
            int i = basePoint.curve_id;
            int j = basePoint.point_id;
            point2D p1 = data[i][j - 1]->point;
            point2D p3 = data[i][j + 1]->point;
            double angle = basePoint.point.get_angle(p1, p3);
            angle = (uint8_t)(angle/30);
            std::cout<<angle<<std::endl;
            val.set(3,  angle);
        }

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
//
//    unsigned beforeNumber;
//    unsigned afterNumber;

    BitMap val;

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
                seed ^= std::size_t(fea.data[i]->point.y) + (seed << 2) + (seed >> 6);
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
//        std::cout << *svgPoint;
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

    SvgAbstractFeature getSubFeature(int i, int j, std::vector<std::vector<svgPoint *>> &data, float distance) {
        svgPoint *point = data[i][j];
//        SvgAbstractFeature res;
        std::vector<svgPoint *> nearPoints = this->rtree.getNearPoints(point, distance);

        SvgAbstractFeature res(nearPoints, *point, data);

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
                symmetries[0] = spatialSvg.getSubFeature(i, j, data, 30.0);
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
            std::vector<svgPoint *> singlePolylinePoint;
            unsigned lenSum = 0;

            std::string &singlePolylineStr = strVector[i];
            std::vector<std::string> vecSegTag = unit::split_str(singlePolylineStr, " ");

            for (int j = 0; j < vecSegTag.size(); j++) {
                std::vector<std::string> pointSeg = unit::split_str(vecSegTag[j], ",");

                point2D tempPoint2D(static_cast<float>(atof(pointSeg[0].c_str())),
                                    static_cast<float>(atof(pointSeg[1].c_str())));
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
        for (unsigned x = 0; x < mat.width; x++) {
            for (unsigned y = 0; y < mat.height; y++) {
                std::cout << mat.get(x, y) << " ";
            }
        }
        std::cout << std::endl;
    };

private:
    std::vector<std::vector<svgPoint *>> data;      //原始的数据
    std::vector<unsigned> len;        //累计的长度列表
    SpatialSvg spatialSvg;                  //封裝好的rtree  svg接口
    unsigned limit;                         //限制距离
};

#endif //SRC_SVG_HPP
