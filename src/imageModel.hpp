#ifndef SRC_IMAGEMODEL_HPP
#define SRC_IMAGEMODEL_HPP
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "data.hpp"
using namespace std;

template<class T ,class ImgAbstractFeature>
class Img : public Data<T,ImgAbstractFeature> {
public:

    void init() {
        initDataWithImg();
//        initDataWithOpencv();
//        initSvgData();
        initPatterns();
        generateCompatible();
    }



    Img(const Options &op) : Data<T,ImgAbstractFeature>(op) {

        init();
    }

    void initDataWithOpencv() {
//        Mat src = imread(this->options.input_data.c_str());
//        assert(!src.empty());
//
//        int row = src.rows;
//        int col = src.cols;
//
//        this->_data = ImgAbstractFeature(row, col);
//
//        for (int i = 0; i < row; i++) {
//            for (int j = 0; j < col; j++) {
//                Vec3b vec_3 = src.at<Vec3b>(i, j);
//                int b = vec_3[0];
//                int g = vec_3[1];
//                int r = vec_3[2];
//                cout << "B:" << b << " G:" << g << " R:" << r << endl;
//                this->_data.data[i * col + j] = Cell(r, g, b);
//            }
//        }
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
                this->_data.data[i * width + j] = Cell(data[index], data[index + 1], data[index + 2]);
            }
        }
        free(data);
    }

    void write_image_png(const std::string &file_path, const ImgAbstractFeature &m) noexcept {
        stbi_write_png(file_path.c_str(), m.width, m.height, 3, (const unsigned char *) m.data.data(), 0);
    }

    static bool isEpual(const ImgAbstractFeature &pattern1, const ImgAbstractFeature &pattern2, Direction Direction) noexcept {
        int dx = Direction.x;
        int dy = Direction.y;

        unsigned xmin = max(dx, 0);
        unsigned xmax = dx < 0 ? dx + pattern2.width : pattern1.width;
        unsigned ymin = max(dy, 0);
        unsigned ymax = dy < 0 ? dy + pattern2.height : pattern1.width;

        // Iterate on every pixel contained in the intersection of the two pattern.
        // 迭代两个图案中每个像素
        for (unsigned y = ymin; y < ymax; y++) {
            for (unsigned x = xmin; x < xmax; x++) {
                // Check if the color is the same in the two patterns in that pixel.
                // 检查是否颜色相同
                if (pattern1.get(y, x) != pattern2.get(y - dy, x - dx)) {
                    return false;
                }
            }
        }
        return true;
    }


    template<typename Func, typename ...Ts>
    void doEveryPatternIdFunc(Func fun, Ts...agv) {
        for (unsigned pattern1 = 0; pattern1 < this->patterns.size(); pattern1++) {
            fun(pattern1, std::forward<Ts>(agv)...);
        }
    }


    /**
* Precompute the function isEpual(pattern1, pattern2, dy, dx).
* If isEpual(pattern1, pattern2, dy, dx), then compatible[pattern1][direction]
* contains pattern2, where direction is the direction defined by (dy, dx) (see direction.hpp).
* 先计算是否匹配
 如果匹配，则合并
*/
    void generateCompatible() noexcept {
        this->propagator = std::vector<std::array<std::vector<unsigned>, 4> >(this->patterns.size());

        //对于每个个图案id ，均执行以下函数
        auto realJob = [&](int pattern2, int pattern1, int directionId) {
            auto Direction = this->_direction.getPoint(directionId);
            if (isEpual(this->patterns[pattern1], this->patterns[pattern2], Direction)) {
                this->propagator[pattern1][directionId].push_back(pattern2);
            }
        };

        auto iterFunc = [&](int directionId, int pattern1) {
            doEveryPatternIdFunc(
                    std::bind(realJob, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), pattern1,
                    directionId);
        };

        auto doDiretFunc = [&](int pattern1) {
            this->_direction.doEveryDirectId(std::bind(iterFunc, std::placeholders::_1, std::placeholders::_2),
                                             pattern1);
        };

        doEveryPatternIdFunc(std::bind(doDiretFunc, std::placeholders::_1));
    }

    void initPatterns() noexcept {
//        std::unordered_map<ImgAbstractFeature, unsigned> patterns_id;
//        std::vector<Matrix<Cell>> symmetries(this->options.symmetry, ImgAbstractFeature(this->options.N, this->options.N));

        std::unordered_map<ImgAbstractFeature, unsigned> patterns_id;
        std::vector<ImgAbstractFeature> symmetries(this->options.symmetry,ImgAbstractFeature(this->options.N, this->options.N));

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

    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    ImgAbstractFeature to_image(const Matrix<unsigned> &output_patterns) const noexcept {
        ImgAbstractFeature output = ImgAbstractFeature(this->options.out_height, this->options.out_width);

        for (unsigned y = 0; y < this->options.wave_height; y++) {
            for (unsigned x = 0; x < this->options.wave_width; x++) {
                output.get(y, x) = this->patterns[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < this->options.wave_height; y++) {
            const ImgAbstractFeature &pattern =
                    this->patterns[output_patterns.get(y, this->options.wave_width - 1)];
            for (unsigned dx = 1; dx < this->options.N; dx++) {
                output.get(y, this->options.wave_width - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < this->options.wave_width; x++) {
            const ImgAbstractFeature &pattern =
                    this->patterns[output_patterns.get(this->options.wave_height - 1, x)];
            for (unsigned dy = 1; dy < this->options.N; dy++) {
                output.get(this->options.wave_height - 1 + dy, x) =
                        pattern.get(dy, 0);
            }
        }
        const ImgAbstractFeature &pattern = this->patterns[output_patterns.get(
                this->options.wave_height - 1, this->options.wave_width - 1)];
        for (unsigned dy = 1; dy < this->options.N; dy++) {
            for (unsigned dx = 1; dx < this->options.N; dx++) {
                output.get(this->options.wave_height - 1 + dy,
                           this->options.wave_width - 1 + dx) = pattern.get(dy, dx);
            }
        }
        return output;
    }

    void showResult(Matrix<unsigned> mat){
        ImgAbstractFeature res;
        res = to_image(mat);
        if (res.data.size() > 0) {
            write_image_png(this->options.output_data, res);
            cout <<" finished!" << endl;
        } else {
            cout << "failed!" << endl;
        }
    };
    // 原始从输入文件读取得到的数据
    ImgAbstractFeature _data;

};

#endif // SRC_IMAGEMODEL_HPP
