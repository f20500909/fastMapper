#ifndef SRC_DECLARE_HPP
#define SRC_DECLARE_HPP

#include <string>
#include <vector>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

using namespace cv;

//相关宏定义
//一个数据单元的方向数量，4表示上下左右四个方向
#define  directionNumbers 4

enum ObserveStatus {
    success, // wfc完成并取得成功
    failure, // wfc完成并失败
    to_continue // wfc没有完成
};

//方向值
class Direction {
public:
    Direction(int x, int y) : x(x), y(y) {
    }

    Direction(unsigned x, unsigned y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {
    }


    int x;
    int y;
};


class CoordinateState {
public:
    CoordinateState(int x, int y) : x(x), y(y) {
    }

    CoordinateState(unsigned x, unsigned y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {
    }

    CoordinateState getNextDirection(const Direction &direction) {
        CoordinateState res(*this);
        res.x += direction.x;
        res.y += direction.y;
        return res;
    }


    CoordinateState operator+(const CoordinateState &coor) {
        CoordinateState res(coor);
        res.x += coor.x;
        res.y += coor.y;
        return res;
    }

    CoordinateState operator-(const CoordinateState &coor) {
        CoordinateState res(coor);
        res.x -= coor.x;
        res.y -= coor.y;
        return res;
    }

    int x;
    int y;
};

/**
 * A direction is represented by an unsigned integer in the range [0; 3].
 * The x and y values of the direction can be retrieved in these tables.
 */
// TODO 方向的遍历写成迭代器方式

/**
 * Return the opposite direction of direction.
 */
constexpr unsigned get_opposite_direction(unsigned direction) noexcept {
    return directionNumbers-1 - direction;
}

struct Options {
    unsigned out_height;
    unsigned out_width;
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    unsigned N; // The width and height in pixel of the patterns.
    std::string input_data; // The width and height in pixel of the patterns.
    std::string output_data; // The width and height in pixel of the patterns.
    std::string type;        // 模式

    unsigned wave_height;  // The height of the output in pixels.
    unsigned wave_width;   // The width of the output in pixels.
    const int directionSize = 4;
    int channels;

    Options() {
    }

    Options(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels,
            std::string input_data, std::string output_data, std::string type) :
            out_height(out_height),
            out_width(out_width),
            symmetry(symmetry),
            N(N),
            input_data(input_data),
            output_data(output_data),
            type(type),
            wave_height(out_height - N + 1),
            wave_width(out_width - N + 1),
            channels(channels) {

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

/**
* Represent a 24-bit rgb color.
*/
class Cell {
public:
    Cell(){
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
    }

    Cell(unsigned char r,unsigned char g,unsigned char b) {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }

    unsigned char data[3];

    bool operator==(const Cell &c) const noexcept {
        for (int i = 0; i < 3; i++) {
            if (data[i] != c.data[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Cell &c) const noexcept { return !(c == *this); }
};

/**
* Represent a 2D array.
* The 2D array is stored in a single array, to improve cache usage.
*/
template<typename T>
class Matrix {

public:
    /**
    * Height and width of the 2D array.
    */
    unsigned height;
    unsigned width;

    /**
    * The array containing the data of the 2D array.
    */
    std::vector<T> data;

    /**
    * Build a 2D array given its height and width.
    * All the array elements are initialized to default value.
    */
    Matrix() {

    };

    Matrix(unsigned height, unsigned width) noexcept : height(height), width(width), data(width * height) {}

    /**
    * Build a 2D array given its height and width.
    * All the array elements are initialized to value.
    */
    Matrix(unsigned height, unsigned width, T value) noexcept
            : height(height), width(width), data(width * height, value) {}

    /**
    * Return a const reference to the element in the i-th line and j-th column.
    * i must be lower than height and j lower than width.
    */
    const T &get(unsigned i, unsigned j) const noexcept {
        assert(i < height && j < width);
        return data[j + i * width];
    }

    const T &getByPoint(Direction op) const noexcept {
        unsigned  i=op.x;
        unsigned  j=op.y;
        return data[j+i*width];
    }

    /**
    * Return a reference to the element in the i-th line and j-th column.
    * i must be lower than height and j lower than width.
    */
    T &get(unsigned i, unsigned j) noexcept {

        assert(i < height && j < width);
        return data[j + i * width];
    }

    /**
    * Return the current 2D array reflected along the x axis.
    */
    Matrix<T> reflected() const noexcept {
        Matrix<T> result = Matrix<T>(width, height);
        for (unsigned y = 0; y < height; y++) {
            for (unsigned x = 0; x < width; x++) {
                result.get(y, x) = get(y, width - 1 - x);
            }
        }
        return result;
    }

    /**
    * Return the current 2D array rotated 90° anticlockwise
    */
    Matrix<T> rotated() const noexcept {
        Matrix<T> result = Matrix<T>(width, height);
        for (unsigned y = 0; y < width; y++) {
            for (unsigned x = 0; x < height; x++) {
                result.get(y, x) = get(x, width - 1 - y);
            }
        }
        return result;
    }

    /**
    * Return the sub 2D array starting from (y,x) and with size (sub_width,
    * sub_height). The current 2D array is considered toric for this operation.
    */
    Matrix<T> get_sub_array(unsigned y, unsigned x, unsigned sub_width,
                            unsigned sub_height) const noexcept {
        Matrix<T> sub_array_2d = Matrix<T>(sub_width, sub_height);
        for (unsigned ki = 0; ki < sub_height; ki++) {
            for (unsigned kj = 0; kj < sub_width; kj++) {
                sub_array_2d.get(ki, kj) = get((y + ki) % height, (x + kj) % width);
            }
        }
        return sub_array_2d;
    }

    /**
    * Assign the matrix a to the current matrix.
    */
    Matrix<T> &operator=(const Matrix<T> &a) noexcept {
        height = a.height;
        width = a.width;
        data = a.data;
        return *this;
    }

    /**
    * Check if two 2D arrays are equals.
    */
    bool operator==(const Matrix<T> &a) const noexcept {
        if (height != a.height || width != a.width) {
            return false;
        }

        for (unsigned i = 0; i < data.size(); i++) {
            if (a.data[i] != data[i]) {
                return false;
            }
        }
        return true;
    }
};



/**
* Hash function for color.
*/
namespace std {
    template<>
    class hash<Cell> {
    public:
        size_t operator()(const Cell &c) const {
            return (size_t) c.data[0] + (size_t) 256 * (size_t) c.data[1] +
                                                                (size_t) 256 * (size_t) 256 * (size_t) c.data[2];
        }
    };

    template<typename T>
    class hash<Matrix<T>> {
    public:
    size_t operator()(const Matrix<T> &a) const noexcept {
        std::size_t seed = a.data.size();
        for (const T &i : a.data) {
            seed ^= hash<T>()(i) + (size_t) 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
}


#endif