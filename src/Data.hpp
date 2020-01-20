#ifndef SRC_DATA_HPP
#define SRC_DATA_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "include/stb_image.h"
#include "include/stb_image_write.h"

#include "Matrix.hpp"
#include "color.hpp"
#include "image.hpp"
#include "declare.hpp"
#include "direction.hpp"


using namespace std;

template<class T>
class Data {
public:

    Data() {
    }

    Data(const Options &options) : options(options) {
        init();
    }

    void initData() {
        int width;
        int height;
        int num_components;
        unsigned char *data = stbi_load(options.image_path.c_str(), &width, &height, &num_components, 3);

        _data = Matrix<Cell>(height, width);
        for (unsigned i = 0; i < (unsigned) height; i++) {
            for (unsigned j = 0; j < (unsigned) width; j++) {
                unsigned index = 3 * (i * width + j);
                _data.data[i * width + j] = Cell(data[index], data[index + 1], data[index + 2]);
            }
        }
        free(data);
    }

    void init() {
        initData();
        init_patterns();
        generate_compatible();
    }

    static void write_image_png(const std::string &file_path, const Matrix<Cell> &m) noexcept {
        stbi_write_png(file_path.c_str(), m.width, m.height, 3, (const unsigned char *) m.data.data(), 0);
    }

    static bool isEpual(const Matrix<Cell> &pattern1, const Matrix<Cell> &pattern2, int dy, int dx) noexcept {
        unsigned xmin = max(0, dx);
        unsigned xmax = dx < 0 ? dx + pattern2.width : pattern1.width;
        unsigned ymin = max(0, dy);
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

    void iterFunc(int pattern1, int direction) {
        for (unsigned pattern2 = 0; pattern2 < patterns.size(); pattern2++) {
            //判断是否相等
            if (isEpual(patterns[pattern1], patterns[pattern2], directions_y[direction], directions_x[direction])) {
                //判断是否相等，如果相等则赋值记录
                propagator[pattern1][direction].push_back(pattern2);
            }
        }
    }

    std::function<void(int, int)> f1 = std::bind(&Data<int>::iterFunc, this, std::placeholders::_1,
                                                 std::placeholders::_2);
//    std::function<void(int)> f3 = std::bind(&Data<int>::patternsFunc, this, std::placeholders::_1);

    void doDiretFunc(int pattern1) {
        for (unsigned direction = 0; direction < 4; direction++) {
            f1(pattern1, direction);
        }
    }

    void doPatternFunc(std::function<void(int)> diretFunc) {
        for (unsigned pattern1 = 0; pattern1 < patterns.size(); pattern1++) {
            // 对上下左右四个方向
            diretFunc(pattern1);
        }
    }

/**
* Precompute the function isEpual(pattern1, pattern2, dy, dx).
* If isEpual(pattern1, pattern2, dy, dx), then compatible[pattern1][direction]
* contains pattern2, where direction is the direction defined by (dy, dx) (see direction.hpp).
* 先计算是否匹配
 如果匹配，则合并
*/
    void generate_compatible() noexcept {
        propagator = std::vector<std::array<std::vector<unsigned>, 4> >(patterns.size());

        auto patternFunc = std::bind(&Data<int>::doPatternFunc, this, std::placeholders::_1);

        patternFunc(std::bind(&Data<int>::doDiretFunc, this, std::placeholders::_1));
    }

    void init_patterns() noexcept {
        std::unordered_map<Matrix<Cell>, unsigned> patterns_id;
        std::vector<Matrix<Cell>> symmetries(8, Matrix<Cell>(options.N, options.N));
        unsigned max_i = _data.height - options.N + 1;
        unsigned max_j = _data.width - options.N + 1;

        for (unsigned i = 0; i < max_i; i++) {
            for (unsigned j = 0; j < max_j; j++) {
                symmetries[0].data = _data.get_sub_array(i, j, options.N, options.N).data;
                symmetries[1].data = symmetries[0].reflected().data;
                symmetries[2].data = symmetries[0].rotated().data;
                symmetries[3].data = symmetries[2].reflected().data;
                symmetries[4].data = symmetries[2].rotated().data;
                symmetries[5].data = symmetries[4].reflected().data;
                symmetries[6].data = symmetries[4].rotated().data;
                symmetries[7].data = symmetries[6].reflected().data;

                for (unsigned k = 0; k < options.symmetry; k++) {
                    auto res = patterns_id.insert(std::make_pair(symmetries[k], patterns.size()));
                    if (!res.second) {
                        patterns_frequency[res.first->second] += 1;
                    } else {
                        patterns.push_back(symmetries[k]);
                        patterns_frequency.push_back(1);
                    }
                }
            }
        }
    }

    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    Matrix<Cell> to_image(const Matrix<unsigned> &output_patterns) const noexcept {
        Matrix<Cell> output = Matrix<Cell>(options.out_height, options.out_width);

        for (unsigned y = 0; y < options.wave_height; y++) {
            for (unsigned x = 0; x < options.wave_width; x++) {
                output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < options.wave_height; y++) {
            const Matrix<Cell> &pattern =
                    patterns[output_patterns.get(y, options.wave_width - 1)];
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(y, options.wave_width - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < options.wave_width; x++) {
            const Matrix<Cell> &pattern =
                    patterns[output_patterns.get(options.wave_height - 1, x)];
            for (unsigned dy = 1; dy < options.N; dy++) {
                output.get(options.wave_height - 1 + dy, x) =
                        pattern.get(dy, 0);
            }
        }
        const Matrix<Cell> &pattern = patterns[output_patterns.get(
                options.wave_height - 1, options.wave_width - 1)];
        for (unsigned dy = 1; dy < options.N; dy++) {
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(options.wave_height - 1 + dy,
                           options.wave_width - 1 + dx) = pattern.get(dy, dx);
            }
        }
        return output;
    }

    //维度
    int dim;

    //每个维度的尺寸大小
    //图像是2维，三维物体是3维
    std::vector<int> dims;

    std::vector<Matrix<Cell>> patterns;

    std::vector<double> patterns_frequency;

    Matrix<Cell> m;
    std::vector<std::array<std::vector<unsigned>, 4>> propagator;
    Matrix<Cell> _data;
    const Options options;
    Direction direct;
};

#endif // SRC_DATA_HPP