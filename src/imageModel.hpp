#ifndef SRC_IMAGEMODEL_HPP
#define SRC_IMAGEMODEL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "declare.hpp"

using namespace std;

template<class T, class ImgAbstractFeature>
class Img : public Data<T, ImgAbstractFeature> {
public:

    void init() {
        initDataWithImg();
        initPatterns();
        generateCompatible();
    }

    Img(const Options &op) : Data<T, ImgAbstractFeature>(op) {
        this->init();
    }

    void initDataWithImg() {
        int width;
        int height;
        int num_components;
        unsigned char *data = stbi_load(this->options.input_data.c_str(), &width, &height, &num_components,
                                        this->options.channels);

        this->_data = ImgAbstractFeature(height, width);
        for (unsigned i = 0; i < (unsigned) height; i++) {
            for (unsigned j = 0; j < (unsigned) width; j++) {
                unsigned index = 3 * (i * width + j);
                this->_data.data[i * width + j] = (data[index]) | ((data[index + 1]) << 8) | ((data[index + 2]) << 16);
            }
        }
        free(data);
    }

    void write_image_png(const std::string &file_path, const ImgAbstractFeature &m) noexcept {

        unsigned char *imgData = new unsigned char[m.width * m.height * 3];
        for (int i = 0; i < m.width * m.height; i++) {
            unsigned t = m.data[i];
            imgData[i * 3 + 0] = (unsigned char) (t & 0xFF);// 0-7位
            imgData[i * 3 + 1] = (unsigned char) ((t & 0xFF00) >> 8);// 8-15位
            imgData[i * 3 + 2] = (unsigned char) ((t & 0xFF0000) >> 16);// 16-23位
        };
        stbi_write_png(file_path.c_str(), m.width, m.height, 3, imgData, 0);
    }

    bool isIntersect(const ImgAbstractFeature &pattern1, const ImgAbstractFeature &pattern2, unsigned directionId) noexcept {
        Direction direction = this->_direction._data[directionId];
        int dx = direction.x;
        int dy = direction.y;

        unsigned xmin = max(dx, 0);
        unsigned xmax = min(pattern2.width + dx, pattern1.width);
        unsigned ymin = max(dy, 0);
        unsigned ymax = min(pattern2.height + dy, pattern1.width);

        // Iterate on every pixel contained in the intersection of the two pattern.
        // 迭代两个图案中每个像素
        for (unsigned y = ymin; y < ymax; y++) {
            for (unsigned x = xmin; x < xmax; x++) {
                // 检查值是否相同

                unsigned x2 = x - dx;
                unsigned y2 = y - dy;

                if (pattern1.get(x + y * pattern2.width) != pattern2.get(x2 + y2 * pattern2.width)) {
                    return false;
                }

            }
        }
        return true;
    }

    void generateCompatible() noexcept {
        //图案id  方向id   此图案此方向同图案的id
        // 是一个二维矩阵  居中中的每个元素为一个非定长数组
        this->propagator = std::vector<std::vector<std::vector<unsigned>>>(this->feature.size(),
                                                                           std::vector<std::vector<unsigned>>(
                                                                                   this->_direction.getMaxNumber())); //每个特征
        for (unsigned pattern1 = 0; pattern1 < this->feature.size(); pattern1++) {
            //每个方向
            for (int directionId = 0; directionId < this->_direction.getMaxNumber(); directionId++) {
                //每个方向的每个特征
                for (unsigned pattern2 = 0; pattern2 < this->feature.size(); pattern2++) {
                    //判断是否相等  相等就压入图案到传播队列
                    if (isIntersect(this->feature[pattern1], this->feature[pattern2], directionId)) {
                        this->propagator[pattern1][directionId].push_back(pattern2);
                    }
                }
            }
        }
    }

    void initPatterns() noexcept {
        std::unordered_map<ImgAbstractFeature, unsigned> patterns_id;
        std::vector<ImgAbstractFeature> symmetries(this->options.symmetry,
                                                   ImgAbstractFeature(this->options.N, this->options.N));

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
                    auto res = patterns_id.insert(std::make_pair(symmetries[k], this->feature.size()));
                    if (!res.second) {
                        this->patterns_frequency[res.first->second] += 1;
                    } else {
                        this->feature.push_back(symmetries[k]);
                        this->patterns_frequency.push_back(1);
                    }
                }
            }
        }
    }

    /**
    * Transform a 2D array containing the feature id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    ImgAbstractFeature to_image(const Matrix<unsigned> &output_patterns) const noexcept {
        ImgAbstractFeature output = ImgAbstractFeature(this->options.out_height, this->options.out_width);

        for (unsigned y = 0; y < this->options.wave_height; y++) {
            for (unsigned x = 0; x < this->options.wave_width; x++) {
                output.get(y, x) = this->feature[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < this->options.wave_height; y++) {
            const ImgAbstractFeature &pattern = this->feature[output_patterns.get(y, this->options.wave_width - 1)];
            for (unsigned dx = 1; dx < this->options.N; dx++) {
                output.get(y, this->options.wave_width - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < this->options.wave_width; x++) {
            const ImgAbstractFeature &pattern = this->feature[output_patterns.get(this->options.wave_height - 1, x)];
            for (unsigned dy = 1; dy < this->options.N; dy++) {
                output.get(this->options.wave_height - 1 + dy, x) = pattern.get(dy, 0);
            }
        }
        const ImgAbstractFeature &pattern = this->feature[output_patterns.get(this->options.wave_height - 1,
                                                                              this->options.wave_width - 1)];
        for (unsigned dy = 1; dy < this->options.N; dy++) {
            for (unsigned dx = 1; dx < this->options.N; dx++) {
                output.get(this->options.wave_height - 1 + dy, this->options.wave_width - 1 + dx) = pattern.get(dy, dx);
            }
        }
        return output;
    }

    void showResult(Matrix<unsigned> mat) {
        ImgAbstractFeature res;
        res = to_image(mat);
        if (res.data.size() > 0) {
            write_image_png(this->options.output_data, res);
            cout << " finished!" << endl;
        } else {
            cout << "failed!" << endl;
        }
    };
    ImgAbstractFeature _data;

};

#endif // SRC_IMAGEMODEL_HPP
