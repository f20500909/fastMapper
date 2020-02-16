#ifndef SRC_DECLARE_HPP
#define SRC_DECLARE_HPP

#include <string>
#include <vector>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>

using namespace cv;

//相关宏定义
//一个数据单元的方向数量，4表示上下左右四个方向
#define  directionNumbers 4

enum ObserveStatus {
    success, // wfc完成并取得成功
    failure, // wfc完成并失败
    to_continue // wfc没有完成
};
//具体的坐标值
typedef std::vector<int> coordinateState;

//方向值
typedef std::vector<int> point;

/**
 * A direction is represented by an unsigned integer in the range [0; 3].
 * The x and y values of the direction can be retrieved in these tables.
 */
// TODO 方向的遍历写成迭代器方式

/**
 * Return the opposite direction of direction.
 */
constexpr unsigned get_opposite_direction(unsigned direction) noexcept {
    return 3 - direction;
}

struct Options {
    std::string type;  // The height of the output in pixels.

    unsigned out_height;  // The height of the output in pixels.
    unsigned out_width;   // The width of the output in pixels.
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    unsigned N; // The width and height in pixel of the patterns.
    std::string input_data; // The width and height in pixel of the patterns.
    std::string output_data; // The width and height in pixel of the patterns.


    unsigned wave_height;  // The height of the output in pixels.

    unsigned wave_width;   // The width of the output in pixels.

    const int directionSize = 4;
    int desired_channels;

    Options() {
    }

    Options(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int _desired_channels,
            std::string input_data, std::string output_data) :
            out_height(out_height),
            out_width(out_width),
            symmetry(symmetry),
            N(N),
            input_data(input_data),
            output_data(output_data),
            wave_height(out_height - N + 1),
            wave_width(out_width - N + 1),
            desired_channels(_desired_channels) {

    }
};


/**
* Struct containing the values needed to compute the entropy of all the cells.
* This struct is updated every time the wave is changed.
* p'(pattern) is equal to patterns_frequency[pattern] if wave.get(cell,
* pattern) is set to true, otherwise 0.
* 结构包含计算所有网格的熵所需的值
* 当波更改每次都会更新次结构
*/
struct Entropy {
    std::vector<double> plogp_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;       // The sum of p'(pattern).
    std::vector<double> log_sum;   // The log of sum.
    std::vector<unsigned> nb_patterns_vec; // The number of patterns present
    std::vector<double> entropy;       // The entropy of the cell.
};


#endif