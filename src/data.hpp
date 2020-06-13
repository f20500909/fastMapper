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
    Data(){

    }
    template<class KEY>
    long long getKey(KEY wave_id, KEY fea_id, KEY direction_id) {
        return wave_id * conf->wave_size + fea_id * feature.size() + direction_id;
    }


    //没找到 就初始化  那就不用在最初进行初始化了 省了很多事
    int &getDirectionCount(const unsigned &wave_id, const unsigned &fea_id, const unsigned &direction) {
        auto iter = compatible_feature_map.find(getKey(wave_id, fea_id, direction));

        if (iter == compatible_feature_map.end()) {
            //一个fea_id和一个direction唯一确定一个方向
            unsigned oppositeDirection = _direction.get_opposite_direction(fea_id, direction);

            //此方向上的值  等于 其反方向上的可传播大小
            long long key = getKey(wave_id, fea_id, direction);
            compatible_feature_map[key] = propagator[fea_id][oppositeDirection].markSize();
            return compatible_feature_map[key];
        }

        return iter->second;
    }

    template< class ImgAbstractFeature>
    void write_image_png(const std::string &file_path, const ImgAbstractFeature &m) noexcept {
        unsigned char *imgData = new unsigned char[m.getHeight() * m.getWidth() * 3];
        for (unsigned i = 0; i < m.getWidth() * m.getHeight(); i++) {
            unsigned t = m.data[i];
            imgData[i * 3 + 0] = (unsigned char) (t & 0xFF);// 0-7位
            imgData[i * 3 + 1] = (unsigned char) ((t & 0xFF00) >> 8);// 8-15位
            imgData[i * 3 + 2] = (unsigned char) ((t & 0xFF0000) >> 16);// 16-23位
        };
        stbi_write_png(file_path.c_str(), m.getWidth(), m.getHeight(), 3, imgData, 0);
    }

    //matrix 写入图像
    Matrix<unsigned> to_image( Matrix<unsigned> output_features) const noexcept {
        Matrix<unsigned> res = Matrix<unsigned>(conf->out_height, conf->out_width);

        //写入主要区域的数据
        for (unsigned y = 0; y < conf->wave_height; y++) {
            for (unsigned x = 0; x < conf->wave_width; x++) {
                res.get(y, x) = feature[output_features.get(y, x)].get(0, 0);
            }
        }
        // 下面的三次写入是处理边缘条件

        //写入左边部分
        for (unsigned y = 0; y < conf->wave_height; y++) {
            const Matrix<unsigned> &fea = feature[output_features.get(y, conf->wave_width - 1)];
            for (unsigned dx = 1; dx < conf->N; dx++) {
                res.get(y, conf->wave_width - 1 + dx) = fea.get(0, dx);
            }
        }

        //写入下边部分
        for (unsigned x = 0; x < conf->wave_width; x++) {
            const Matrix<unsigned> &fea = feature[output_features.get(conf->wave_height - 1, x)];
            for (unsigned dy = 1; dy < conf->N; dy++) {
                res.get(conf->wave_height - 1 + dy, x) = fea.get(dy, 0);
            }
        }

        //写入右下角的一小块
        const Matrix<unsigned> &fea = feature[output_features.get(conf->wave_height - 1,
                                                                    conf->wave_width - 1)];
        for (unsigned dy = 1; dy < conf->N; dy++) {
            for (unsigned dx = 1; dx < conf->N; dx++) {
                res.get(conf->wave_height - 1 + dy, conf->wave_width - 1 + dx) = fea.get(dy, dx);
            }
        }
        return res;
    }


};

#endif // SRC_DATA_HPP