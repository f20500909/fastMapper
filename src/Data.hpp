#ifndef INPUT_HPP
#define INPUT_HPP

#include <vector>
#include <string>
#include <algorithm>

#include "include/stb_image.h"
#include "include/stb_image_write.h"

#include "Matrix.hpp"
#include "color.hpp"
#include "overlapping_wfc.hpp"
#include "image.hpp"

using namespace std;

template<class T>
class Data {
public:
//    static struct type {
//        int dim = 2;
//    };

    Data() {

    }

    void getData() {

    }

    void parse() {

    }

    std::optional<Matrix<Color>> init(std::string image_path) {
        int width;
        int height;
        int num_components;
        unsigned char *data = stbi_load(image_path.c_str(), &width, &height, &num_components, 3);
        if (data == nullptr) {
            return std::nullopt;
        }
        m = Matrix<Color>(height, width);
        for (unsigned i = 0; i < (unsigned) height; i++) {
            for (unsigned j = 0; j < (unsigned) width; j++) {
                unsigned index = 3 * (i * width + j);
                m.data[i * width + j] = Color(data[index], data[index + 1], data[index + 2]);
            }
        }
        free(data);
        return m;
    }

    static void write_image_png(const std::string &file_path, const Matrix<Color> &m) noexcept {
        stbi_write_png(file_path.c_str(), m.width, m.height, 3, (const unsigned char *) m.data.data(), 0);
    }

/**
* Return true if the pattern1 is compatible with pattern2
* when pattern2 is at a distance (dy,dx) from pattern1.
* 当两个图案距离dy，dx时检测是否匹配，在此距离下是否相等
*/
    static bool isEpual(const Matrix<Color> &pattern1, const Matrix<Color> &pattern2, int dy, int dx) noexcept {
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

/**
* Precompute the function isEpual(pattern1, pattern2, dy, dx).
* If isEpual(pattern1, pattern2, dy, dx), then compatible[pattern1][direction]
* contains pattern2, where direction is the direction defined by (dy, dx) (see direction.hpp).
* 先计算是否匹配
 如果匹配，则合并
*/
    void generate_compatible() noexcept {
        propagator = std::vector<
                std::array< std::vector<unsigned>, 4>
        >(patterns.size());
        // Iterate on every dy, dx, pattern1 and pattern2
        // 对每个图案
        for (unsigned pattern1 = 0; pattern1 < patterns.size(); pattern1++) {
            // 对上下左右四个方向
            for (unsigned direction = 0; direction < 4; direction++) {
                // 对所需要比较的每个图案
                for (unsigned pattern2 = 0; pattern2 < patterns.size(); pattern2++) {
                    //判断是否相等
                    if (isEpual(patterns[pattern1], patterns[pattern2], directions_y[direction],
                                directions_x[direction])) {
                        //判断是否相等，如果相等则赋值记录
                        propagator[pattern1][direction].push_back(pattern2);
                    }
                }
            }
        }
    }

/**
* Return the list of patterns, as well as their probabilities of apparition.
* 返回图案列表，以及它出现的概率
*/
    void get_patterns(const Matrix<Color> &input, const OverlappingWFCOptions &options) noexcept {
        std::unordered_map<Matrix<Color>, unsigned> patterns_id;

        // The number of time a pattern is seen in the input image.
        // 一个图案在输入中出现的次数

        std::vector<Matrix<Color>> symmetries(8, Matrix<Color>(options.N, options.N));
        unsigned max_i = input.height - options.N + 1;
        unsigned max_j = input.width - options.N + 1;

        for (unsigned i = 0; i < max_i; i++) {
            for (unsigned j = 0; j < max_j; j++) {
                // Compute the symmetries of every pattern in the image.
                // 计算此图案的其他形式，旋转，对称
                symmetries[0].data = input.get_sub_array(i, j, options.N, options.N).data;
                symmetries[1].data = symmetries[0].reflected().data;
                symmetries[2].data = symmetries[0].rotated().data;
                symmetries[3].data = symmetries[2].reflected().data;
                symmetries[4].data = symmetries[2].rotated().data;
                symmetries[5].data = symmetries[4].reflected().data;
                symmetries[6].data = symmetries[4].rotated().data;
                symmetries[7].data = symmetries[6].reflected().data;

                // The number of symmetries in the option class define which symetries will be used.
                // 哪些对称将被使用
                for (unsigned k = 0; k < options.symmetry; k++) {
                    auto res = patterns_id.insert(
                            std::make_pair(symmetries[k], patterns.size()));

                    // If the pattern already exist, we just have to increase its number of appearance.
                    // 如果图案已经存在，我们只需提高他的出现率
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


    std::vector<T> _data;
    //维度
    int dim;

    //每个维度的尺寸大小
    //图像是2维，三维物体是3维
    std::vector<int> dims;

    std::vector<Matrix<Color>> patterns;

    std::vector<double> patterns_frequency;

    Matrix<Color> m;
    std::vector<std::array<std::vector<unsigned>, 4>> propagator;
};

#endif // INPUT_HPP