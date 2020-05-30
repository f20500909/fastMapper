#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "declare.hpp"
//#include "MyRtree.hpp"

using namespace std;

template<typename T>
class Matrix;

template<class T, class AbstractFeature>
class Data {
public:
    Data()  {}

    virtual void showResult(Matrix<unsigned> mat) {
        std::cout << "Data row func err res.." << std::endl;
    };

    bool isVaildPatternId(unsigned pId) {
        unsigned y = pId / conf->wave_width;
        unsigned x = pId % conf->wave_width;

        if (x < 0 || x >= (int) conf->wave_width) {
            return false;
        }
        if (y < 0 || y >= (int) conf->wave_height) {
            return false;
        }
        return true;
    }


    long long getKey(unsigned wave_id, unsigned fea_id) {
        return wave_id * conf->wave_size + fea_id;
    }

    template<class KEY>
    long long getKey(KEY wave_id, KEY fea_id, KEY direction_id) {
        return wave_id * conf->wave_size + fea_id * this->feature.size() + direction_id;
    }

    DirectionSet _direction = DirectionSet(8);

//    std::vector<std::vector<svgPoint *>> data;      //原始的数据
    std::vector<std::vector<BitMap>> propagator;
    std::vector<AbstractFeature> feature;                             //图案数据
    std::vector<unsigned> features_frequency;                            //图案频率


};

#endif // SRC_DATA_HPP