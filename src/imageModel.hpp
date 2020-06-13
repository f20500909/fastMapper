#ifndef SRC_IMAGEMODEL_HPP
#define SRC_IMAGEMODEL_HPP

#include "declare.hpp"
#include "wfc.hpp"
#include <bitset>

using namespace std;


template<class T, class ImgAbstractFeature>
class Img : public WFC {
public:
    ImgAbstractFeature _data;

    void init_input_data() {
        init_direction();
        init_row_data();
        init_features();
        init_compatible();
    }

    void init_direction() {

        _direction._direct = {{0,  1},
                              {1,  0},
                              {0,  -1},
                              {-1, 0},
        };
    }

    void init_row_data() {
        int width;
        int height;
        int num_components;
        unsigned char *data = stbi_load(conf->input_data.c_str(), &width, &height, &num_components,
                                        conf->channels);

        this->_data = ImgAbstractFeature(height, width);
        for (unsigned i = 0; i < (unsigned) height; i++) {
            for (unsigned j = 0; j < (unsigned) width; j++) {
                unsigned index = 3 * (i * width + j);
                this->_data.get(i * width + j) = (data[index]) | ((data[index + 1]) << 8) | ((data[index + 2]) << 16);
            }
        }
        free(data);
        cout << "read img success..." << endl;
        cout << "input img width  " << width << "  height  " << width << "  num_components  " << num_components << endl;
    }

    // 此函数用于判断两个特征 在某个方向上的重叠部分 是否完全相等
    // 重叠部分 全都都相等 才返回true
    bool isIntersect(const ImgAbstractFeature &feature1, const ImgAbstractFeature &feature2, unsigned directionId) noexcept {
//        std::pair<int, int> direction = _direction._data[directionId];
        int dx = _direction.getX(directionId);
        int dy = _direction.getY(directionId);

        unsigned xmin = max(dx, 0);
        unsigned xmax = min(feature2.getWidth() + dx, feature1.getWidth());
        unsigned ymin = max(dy, 0);
        unsigned ymax = min(feature2.getHeight() + dy, feature1.getWidth());

        // 以第一个特征为比较对象 比较每个重叠的元素
        for (unsigned y = ymin; y < ymax; y++) {
            for (unsigned x = xmin; x < xmax; x++) {
                // 检查值是否相同

                unsigned x2 = x - dx;
                unsigned y2 = y - dy;

                if (feature1.get(x + y * feature2.getWidth()) != feature2.get(x2 + y2 * feature2.getWidth())) {
                    return false;
                }

            }
        }
        return true;
    }

    void init_compatible() noexcept {
        //图案id  方向id   此图案此方向同图案的id
        // 是一个二维矩阵  居中中的每个元素为一个非定长数组
        //记录了一个特征在某一个方向上是否能进行传播
        propagator =
                std::vector<std::vector<BitMap>>
                        (feature.size(),
                         vector<BitMap>(_direction.getMaxNumber(), BitMap(feature.size())));

        long long cnt = 0;
        for (unsigned feature1 = 0; feature1 < feature.size(); feature1++) {
            //每个方向
            for (unsigned directionId = 0; directionId < _direction.getMaxNumber(); directionId++) {
                //每个方向的所有特征 注意  需要遍历所有特征 这里的特征已经不包含位置信息了
                for (unsigned feature2 = 0; feature2 < feature.size(); feature2++) {

                    BitMap &temp2 = propagator[feature1][directionId];

                    //判断是否相等  相等就压入图案到传播队列
                    if (isIntersect(feature[feature1], feature[feature2], directionId)) {

                        temp2.set(feature2, true);
                        cnt++;
                    }
                }
            }
        }

        cout << "feature1 size  " << feature.size() << "  max direction number "
             << _direction.getMaxNumber()
             << " propagator count  " << cnt
             << endl;
    }

    void init_features() noexcept {
        std::unordered_map<ImgAbstractFeature, unsigned> features_id;
        std::vector<ImgAbstractFeature> symmetries(conf->symmetry,
                                                   ImgAbstractFeature(conf->N, conf->N));

        unsigned max_i = this->_data.getHeight() - conf->N + 1;
        unsigned max_j = this->_data.getWidth() - conf->N + 1;

        for (unsigned i = 0; i < max_i; i++) {
            for (unsigned j = 0; j < max_j; j++) {
                symmetries[0].data = this->_data.get_sub_array(i, j, conf->N, conf->N).data;
                //TODO 优化镜像的生成过程
                if (1 < conf->symmetry) symmetries[1].data = symmetries[0].reflected().data;
                if (2 < conf->symmetry) symmetries[2].data = symmetries[0].rotated().data;
                if (3 < conf->symmetry) symmetries[3].data = symmetries[2].reflected().data;
                if (4 < conf->symmetry) symmetries[4].data = symmetries[2].rotated().data;
                if (5 < conf->symmetry) symmetries[5].data = symmetries[4].reflected().data;
                if (6 < conf->symmetry) symmetries[6].data = symmetries[4].rotated().data;
                if (7 < conf->symmetry) symmetries[7].data = symmetries[6].reflected().data;

                for (unsigned k = 0; k < conf->symmetry; k++) {
                    auto res = features_id.insert(std::make_pair(symmetries[k], feature.size()));
                    if (!res.second) {
                        features_frequency[res.first->second] += 1;
                    } else {
                        feature.push_back(symmetries[k]);
                        features_frequency.push_back(1);
                    }
                }
            }
        }

        cout << "features size  " << feature.size() << "  features_frequency size "
             << features_frequency.size()
             << endl;
    }

    void show_result(const Matrix<unsigned>& mat) {
        ImgAbstractFeature res = data.to_image(mat);
        if (res.data.size() > 0) {
            this->data.write_image_png(conf->output_data, res);
            cout << " finished!" << endl;
        } else {
            cout << "failed!" << endl;
        }
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


};

#endif // SRC_IMAGEMODEL_HPP
