#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <regex>

#include "declare.hpp"
#include "MyRtree.hpp"
#include "unit.hpp"
#include "bitMap.hpp"
#include "./include/simple_svg.hpp"

class MyRtree;


//特征单元 波函数塌陷的最小计算单元
class SvgAbstractFeature {
public:

    SvgAbstractFeature() {
    }


    SvgAbstractFeature(const SvgAbstractFeature &svg) {
        SvgAbstractFeature res;
        this->isBegin = svg.isBegin;
        this->isEnd = svg.isEnd;
        this->sameCurvePointNumber = svg.sameCurvePointNumber;
        this->diffCurvePointNumber = svg.diffCurvePointNumber;
        this->angleLeft = svg.angleLeft;
        this->angleRight = svg.angleRight;
        this->distanceLeft = svg.distanceLeft;
        this->distanceRight = svg.distanceRight;
        this->shiftLeft = svg.shiftLeft;
        this->shiftRight = svg.shiftRight;
        this->distanceRatio = svg.distanceRatio;
//        this->neighborIds = svg.neighborIds;
        this->direction_fea_id_vec=svg.direction_fea_id_vec;


        this->val = svg.val;
        this->basePoint = svg.basePoint;
        this->data = svg.data;
    }


    SvgAbstractFeature(std::vector<svgPoint *> nearPoints, svgPoint basePoint,
                       std::vector<std::vector<svgPoint *>> &allSvgData) : basePoint(basePoint) {

        // 把对此点的相对坐标写入  data 作为特征数据
        for (int i = 0; i < nearPoints.size(); i++) {
            svgPoint *temp = nearPoints[i];
            temp->point.x -= basePoint.point.x;
            temp->point.y -= basePoint.point.y;
            this->data.push_back(temp);

            if (temp->id != basePoint.id) {
                this->neighborIds.push_back(temp->id);
            }
        }

        this->reSetVal(allSvgData);
    }

    //得到镜像图形
    SvgAbstractFeature reflected() {
        return SvgAbstractFeature(*this);
    }

    //得到旋转后的图形
    SvgAbstractFeature rotated() {
        return SvgAbstractFeature(*this);
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
            this->isBegin = true;
        }
        //终止点判定
        for (int i = 0; i < data.size(); i++) {
            int len = data[i].size();
            if (basePoint.point_id == data[i][len - 1]->point_id) {
                val.set(1, 1);
                this->isEnd = true;
            }
        }

        //同线段上的临近点数量
        int _sameCurvePointNumber = 0;
        for (int i = 0; i < this->data.size(); i++) {
            if (this->data[i]->curve_id == basePoint.curve_id) {
                _sameCurvePointNumber++;
            }
        }

        _sameCurvePointNumber = uint8_t(_sameCurvePointNumber);
        _sameCurvePointNumber = std::min(_sameCurvePointNumber, 0xff);

        val.setNumber(1, _sameCurvePointNumber);
        this->sameCurvePointNumber = _sameCurvePointNumber;


        //不同线段上的临近点数量
        int _diffCurvePointNumber = data.size() - _sameCurvePointNumber;

        val.set(2, _diffCurvePointNumber);

        this->diffCurvePointNumber = _diffCurvePointNumber;

        //角度掩码
        //如果是起点或者终点，角度默认设为0
        //既不是起点 也不是终点
        if (!val.get(0) && !val.get(1)) {
//            float _angle = basePoint.point.get_angle(p1, p3);
//            _angle = (int8_t) (_angle / 30);
//            std::cout << _angle << std::endl;
//            val.set(3, _angle);
//            this->angle = _angle;
        }

        int i = basePoint.curve_id;
        int j = basePoint.point_id;
        //如果不是起点,说明左边有点
        if (!this->isBegin) {
            point2D left = data[i][j - 1]->point;
            point2D temp = point2D(basePoint.point.x, basePoint.point.y + 1000);

            float _angle = basePoint.point.get_angle(left, temp);
            _angle = (int8_t) (_angle / 30);

            this->angleLeft = _angle;

            this->distanceLeft = basePoint.point.get_distance(left);
            this->shiftLeft = basePoint.point.getPointShift(left);

            std::cout << " i " << i << " j " << j << " x " << shiftLeft.x << " y " << shiftLeft.y << std::endl;
        }

        //如果不是终点,说明右边有点
        if (!this->isEnd) {
            point2D right = data[i][j + 1]->point;
            point2D temp = point2D(basePoint.point.x, basePoint.point.y + 1000);
            float _angle = basePoint.point.get_angle(right, temp);
            _angle = (int8_t) (_angle / 30);
            this->angleLeft = _angle;

            this->distanceRight = basePoint.point.get_distance(right);
            this->shiftRight = basePoint.point.getPointShift(right);
        }

    }

    // 特征值属性
    bool isBegin = false;
    bool isEnd = false;
    int sameCurvePointNumber;
    int diffCurvePointNumber;

    int8_t angleLeft = 0;
    int8_t angleRight = 0;

    float distanceLeft = 0;
    float distanceRight = 0;

    point2D shiftLeft = point2D(0, 0);
    point2D shiftRight = point2D(0, 0);


    float distanceRatio;

    std::vector<int> neighborIds;
    std::vector<int> direction_fea_id_vec;

    BitMap val;

    svgPoint basePoint;

    std::vector<svgPoint *> data;
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
    }

    void insert(svgPoint *svgPoint) {
        this->rtree.insert(svgPoint); // Note, all values including zero are fine in this version
//        std::cout << *svgPoint;
    }

    MyRtree rtree;
};

bool operator==(SvgAbstractFeature left, SvgAbstractFeature right) {
    if (left.isBegin != right.isBegin) { return false; }
    if (left.isEnd != right.isEnd) { return false; }
    if (left.angleLeft != right.angleLeft) { return false; }
    if (left.angleRight != right.angleRight) { return false; }
    if (left.sameCurvePointNumber != right.sameCurvePointNumber) { return false; }
    if (left.diffCurvePointNumber != right.diffCurvePointNumber) { return false; }
    return true;
}

template<class T, class AbstractFeature>
class Data;

class Config;

template<class T, class AbstractFeature>
class Svg : public Data<T, AbstractFeature> {
public:

    AbstractFeature getSubFeature(SpatialSvg& spatialSvg, int i, int j, std::vector<std::vector<svgPoint *>> &data) {
        svgPoint *point = data[i][j];
        std::vector<svgPoint *> nearPoints = spatialSvg.rtree.get_nearest(point); // 拿到最近的几个点了  依据这个生成方向数组
        //遍历所有附件的点位  拿到方向值
        vector<int> direction_fea_id_vec = vector<int>(8, -1);
        for (unsigned i = 0; i < nearPoints.size(); i++) {
            point2D po = nearPoints[i]->point;
            float _angle = (*point).point.get_azimuth(po);
            int directionn_id = this->_direction.get_angle_direction_id(_angle, false);
            direction_fea_id_vec[directionn_id] = nearPoints[i]->id;
        }

        AbstractFeature res(nearPoints, *point, data);
        res.direction_fea_id_vec = direction_fea_id_vec;

        return res;
    }

    Svg(const Config &op) : Data<T, AbstractFeature>(op) {
        initDirection();
        parseData();
        initfeatures();
        generateCompatible();
    }

    void initDirection() {

    }

    void initfeatures() {
        // 将图案插入到rtree中
        unsigned id = 0;
        for (int i = 0; i < this->data.size(); i++) {
            for (unsigned j = 0; j < this->data[i].size(); j++) {
                spatialSvg.insert(this->data[i][j]);
            }
        }

        // 获取一个点临近的点位，点位附近的最近点做为特征图案
        std::unordered_map<SvgAbstractFeature, unsigned> features_id;
        std::vector<SvgAbstractFeature> symmetries(8);

        for (int i = 0; i < this->data.size(); i++) {
            for (unsigned j = 0; j < this->data[i].size(); j++) {
                symmetries[0] = this->getSubFeature(spatialSvg,i, j, this->data);
//                symmetries[1] = symmetries[0].reflected();
//                symmetries[2] = symmetries[0].rotated();
//                symmetries[3] = symmetries[2].reflected();
//                symmetries[4] = symmetries[2].rotated();
//                symmetries[5] = symmetries[4].reflected();
//                symmetries[6] = symmetries[4].rotated();
//                symmetries[7] = symmetries[6].reflected();

                for (unsigned k = 0; k < conf->symmetry; k++) {
                    auto res = features_id.insert(std::make_pair(symmetries[k], this->feature.size()));
                    if (!res.second) {
                        this->features_frequency[res.first->second] += 1;
                    } else {
                        this->feature.push_back(symmetries[k]);
                        this->features_frequency.push_back(1);
                    }
                }
            }
        }
    }


    bool isIntersect(const SvgAbstractFeature &feature1, const SvgAbstractFeature &feature2, unsigned directionId) {
        // 找到fea 的 directionId的点位
        std::vector<svgPoint *> svgPoints_1;
        std::vector<svgPoint *> svgPoints_2;

        for (int i = 0; i < feature1.data.size(); i++) {
            //计算点位的方位角
            point2D po = feature1.data[i]->point;
            float _angle = feature1.basePoint.point.get_azimuth(po);

            if (this->_direction.get_angle_direction_id(_angle, false) == directionId) {
                svgPoints_1.push_back(feature1.data[i]);
            }
        }

        // 找到fea 反方向的元素
        for (int i = 0; i < feature2.data.size(); i++) {
            //计算点位的方位角
            point2D po = feature2.data[i]->point;
            float _angle = feature2.basePoint.point.get_azimuth(po);

            if (this->_direction.get_angle_direction_id(_angle, true) == directionId) {
                svgPoints_2.push_back(feature2.data[i]);
            }
        }

        //首先判断点位数量是否相等
        if (svgPoints_1.size() != 0 && svgPoints_1.size() == svgPoints_2.size()) {
            return false;
        }

        //进行搜寻 如果集合1中的每个元素都在集合2中出现 就返回true 否则返回false
        //TODO 在构建集合2的时候就做次判定
        for (int i = 0; i < svgPoints_1.size(); i++) {
            svgPoint *temp = svgPoints_1[i];
            bool isFind = false;
            for (int j = 0; j < svgPoints_2.size(); j++) {
                if (svgPoints_2[j] == temp) {
                    isFind = true;
                }
            }
            if (!isFind) {
                return false;
            }
        }
        return true;
    }

    void generateCompatible() noexcept {
        this->propagator = std::vector<std::vector<std::vector<unsigned>>>(this->feature.size(),
                                                                           std::vector<std::vector<unsigned>>(this->_direction.getMaxNumber()));

        //对每个特征元素
        for (unsigned feature1 = 0; feature1 < this->feature.size(); feature1++) {
            // 应查询此点的

            //对每个特征元素  的 每个邻居
            for (unsigned directionId = 0; directionId < this->_direction.getMaxNumber(); directionId++) {

                //对每个特征元素  的 每个邻居  的每个特征元素
                for (unsigned feature2 = 0; feature2 < this->feature.size(); feature2++) {
                    // 判断在此方向上是否有重叠部分
                    if (this->isIntersect(this->feature[feature1], this->feature[feature2], directionId)) {
                        this->propagator[feature1][directionId].push_back(feature2);
                    }
                }
            }
        }
    }

    void parseData() {
        std::vector<std::string> tmp = get_svg_data(conf->input_data);
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

            std::string &singlePolylineStr = strVector[i];
            std::vector<std::string> vecSegTag = unit::split_str(singlePolylineStr, " ");

            for (int j = 0; j < vecSegTag.size(); j++) {
                std::vector<std::string> pointSeg = unit::split_str(vecSegTag[j], ",");

                point2D tempPoint2D(static_cast<float>(atof(pointSeg[0].c_str())),
                                    static_cast<float>(atof(pointSeg[1].c_str())));
                svgPoint *tempSvgPoint = new svgPoint(tempPoint2D, i, j, cnt++);
                singlePolylinePoint.push_back(tempSvgPoint);
            }
            this->data.push_back(singlePolylinePoint);
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
        svg::Dimensions dimensions(100, 110);
        svg::Document doc("../res/res.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));
        unsigned cnt = 0;
        for (unsigned x = 0; x < mat.width; x++) {

            svg::Polyline polyline_a(svg::Stroke(.5, svg::Color::Blue));

            //第一个点写为基准点
//            polyline_a << svg::Point(0, 0);
            point2D curPoint(0, 0);

            polyline_a << svg::Point(curPoint.x, curPoint.y);
            //在基准点后写入点位

            for (unsigned y = 0; y < mat.height; y++) {

                AbstractFeature fea = this->feature[mat.get(y, x)];

                curPoint.shiftFormPoint(fea.shiftLeft);

                polyline_a << svg::Point(curPoint.x, curPoint.y);
                std::cout << "w x " << curPoint.x << "w y " << curPoint.y << std::endl;
            }

            doc << polyline_a;
        }

        doc.save();

    };

private:
    SpatialSvg spatialSvg;                        //封裝好的rtree  svg接口
};

#endif //SRC_SVG_HPP
