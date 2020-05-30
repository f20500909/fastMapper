#ifndef SRC_IMAGEMODEL_HPP
#define SRC_IMAGEMODEL_HPP

#include "declare.hpp"
#include <bitset>

using namespace std;

template<class T, class ImgAbstractFeature>
class Img : public Data<T, ImgAbstractFeature> {
public:
    ImgAbstractFeature _data;

    void init() {
        initDirection();
        initDataWithImg();
        initfeatures();
        generateCompatible();
    }

    void initDirection() {

        this->_direction.getDirect() = {{0,  1},
                                        {1,  0},
                                        {0,  -1},
                                        {-1, 0},
        };
    }

    Img() : Data<T, ImgAbstractFeature>() {
        this->init();
    }

    void initDataWithImg() {
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

    // 此函数用于判断两个特征 在某个方向上的重叠部分 是否完全相等
    // 重叠部分 全都都相等 才返回true

    bool
    isIntersect(const ImgAbstractFeature &feature1, const ImgAbstractFeature &feature2, unsigned directionId) noexcept {
//        std::pair<int, int> direction = this->_direction._data[directionId];
        int dx = this->_direction.getX(directionId);
        int dy = this->_direction.getY(directionId);

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

    void generateCompatible() noexcept {
        //图案id  方向id   此图案此方向同图案的id
        // 是一个二维矩阵  居中中的每个元素为一个非定长数组
        //记录了一个特征在某一个方向上是否能进行传播
        this->propagator =
                std::vector<std::vector<BitMap>>
                        (this->feature.size(),vector<BitMap>(this->_direction.getMaxNumber(),BitMap(this->feature.size())) );

        long long cnt = 0;
        for (unsigned feature1 = 0; feature1 < this->feature.size(); feature1++) {
            //每个方向
            for (unsigned directionId = 0; directionId < this->_direction.getMaxNumber(); directionId++) {
                //每个方向的所有特征 注意  需要遍历所有特征 这里的特征已经不包含位置信息了
                for (unsigned feature2 = 0; feature2 < this->feature.size(); feature2++) {

                    BitMap& temp2 =  this->propagator[feature1][directionId];

                    //判断是否相等  相等就压入图案到传播队列
                    if (isIntersect(this->feature[feature1], this->feature[feature2], directionId)) {

                        temp2.set(feature2,true);
                        cnt++;
                    }
                }
            }
        }

        cout << "feature1 size  " << this->feature.size() << "  max direction number "
             << this->_direction.getMaxNumber()
             << " propagator count  " << cnt
             << endl;
    }

    void initfeatures() noexcept {
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

        cout << "features size  " << this->feature.size() << "  features_frequency size "
             << this->features_frequency.size()
             << endl;
    }

    /**
    * Transform a 2D array containing the feature id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    ImgAbstractFeature to_image(const Matrix<unsigned> &output_features) const noexcept {
        ImgAbstractFeature res = ImgAbstractFeature(conf->out_height, conf->out_width);

        //写入主要区域的数据
        for (unsigned y = 0; y < conf->wave_height; y++) {
            for (unsigned x = 0; x < conf->wave_width; x++) {
                res.get(y, x) = this->feature[output_features.get(y, x)].get(0, 0);
            }
        }
        // 下面的三次写入是处理边缘条件

        //写入左边部分
        for (unsigned y = 0; y < conf->wave_height; y++) {
            const ImgAbstractFeature &fea = this->feature[output_features.get(y, conf->wave_width - 1)];
            for (unsigned dx = 1; dx < conf->N; dx++) {
                res.get(y, conf->wave_width - 1 + dx) = fea.get(0, dx);
            }
        }

        //写入下边部分
        for (unsigned x = 0; x < conf->wave_width; x++) {
            const ImgAbstractFeature &fea = this->feature[output_features.get(conf->wave_height - 1, x)];
            for (unsigned dy = 1; dy < conf->N; dy++) {
                res.get(conf->wave_height - 1 + dy, x) = fea.get(dy, 0);
            }
        }

        //写入右下角的一小块
        const ImgAbstractFeature &fea = this->feature[output_features.get(conf->wave_height - 1,
                                                                          conf->wave_width - 1)];
        for (unsigned dy = 1; dy < conf->N; dy++) {
            for (unsigned dx = 1; dx < conf->N; dx++) {
                res.get(conf->wave_height - 1 + dy, conf->wave_width - 1 + dx) = fea.get(dy, dx);
            }
        }
        return res;
    }


    void showResult(Matrix<unsigned> mat) {
        ImgAbstractFeature res = to_image(mat);
        if (res.data.size() > 0) {
            write_image_png(conf->output_data, res);
            cout << " finished!" << endl;
        } else {
            cout << "failed!" << endl;
        }
    };


//    void showResult2(Wave& wave) {
//        ImgAbstractFeature res = to_image2(wave);
//        if (res.data.size() > 0) {
//            write_image_png(conf->output_data, res);
//            cout << " finished!" << endl;
//        } else {
//            cout << "failed!" << endl;
//        }
//    };

};

#endif // SRC_IMAGEMODEL_HPP
