#ifndef SRC_DECLARE_HPP
#define SRC_DECLARE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION

#include "include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image_write.h"


template<typename T>
class Matrix;

class SvgAbstractFeature;


enum ObserveStatus {
    success, // wfc完成并取得成功
    failure, // wfc完成并失败
    to_continue // wfc没有完成
};


class Options {
public:
    const unsigned out_height;
    const unsigned out_width;
    const unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    const unsigned N; // The width and height in pixel of the feature.
    const std::string input_data; // The width and height in pixel of the feature.
    const std::string output_data; // The width and height in pixel of the feature.
    const std::string type;        // 模式

    const unsigned wave_height;  // The height of the output in pixels.
    const unsigned wave_width;   // The width of the output in pixels.

    const unsigned wave_size;   // The width of the output in pixels.
    const int directionSize = 4;
    const int channels;

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
            channels(channels),
            wave_size(wave_height * wave_width) {}
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

//需要弱化方向的概念 让方向与模型适配
class DirectionSet {
public:

    DirectionSet(int directionNumbers) : directionNumbers(directionNumbers), increment_angle(360.0 / directionNumbers) {


        //初始化方向的方向
        initDirectionWithAngle();
    }

    //初始化角度
    void initDirectionWithAngle() {
        _data_angle = std::vector<std::pair<float, float>>(directionNumbers, {0, 0});
        _data_opp_angle = std::vector<std::pair<float, float>>(directionNumbers, {0, 0});
        for (unsigned i = 0; i < directionNumbers; i++) {
            _data_angle[i] = {i * increment_angle, (i + 1) * increment_angle};
        }

        for (unsigned i = 0; i < directionNumbers; i++) {
            unsigned index = get_opposite_direction(i);
            _data_opp_angle[i] = _data_angle[index];
        }
    }


    std::vector<std::pair<int, int>> _data = {{0,  -1},
                                              {-1, 0},
                                              {1,  0},
                                              {0,  1}};

    std::vector<std::pair<float, float>> _data_angle; // 角度方向数组   <角度上限，角度下限>  依次规律递增

    std::vector<std::pair<float, float>> _data_opp_angle; // 角度反方向数组   <角度上限，角度下限>  依次规律递增

    std::pair<float, float> get_angle(unsigned direction) {
        return _data_angle[direction];
    }

    std::pair<float, float> get_opp_angle(unsigned direction) {
        return _data_opp_angle[direction];
    }

    unsigned get_opposite_direction(unsigned id) noexcept {
        return ((directionNumbers >> 1) + id) % directionNumbers;
    }

    unsigned getMaxNumber() {
        return directionNumbers;
    }

    unsigned get_feature_id_by_direction(unsigned feature_id, unsigned direction_id) {
        return feature_id + direction_id;
    }

    int get_angle_direction_id(float angle, bool is_opp) {
        return std::min(angle / increment_angle, static_cast<float>(directionNumbers - 1));

//        float min_angle = get_angle(direction_id).first;
//        float max_angle = get_angle(direction_id).second;
//        if (is_opp) {
//            min_angle = get_opp_angle(direction_id).first;
//            max_angle = get_opp_angle(direction_id).second;
//        }
//        return angle >= min_angle && angle < max_angle;
    }

    const float increment_angle;  // 360除以 8   每个方向即是45度
    const int directionNumbers = 8;
};

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


    T &get(unsigned id) noexcept {
        assert(id < height * width);
        return data[id];
    }

    const T &get(unsigned id) const noexcept {
        assert(id < height * width);
        return data[id];
    }


    const T &get(unsigned i, unsigned j) const noexcept {
        assert(i < height && j < width);
        return data[j + i * width];
    }

    const T &getByPoint(Direction op) const noexcept {
        unsigned i = op.x;
        unsigned j = op.y;
        return data[j + i * width];
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

namespace std {
    template<typename T>
    class hash<Matrix<T>> {
    public:
        size_t operator()(const Matrix<T> &a) const noexcept {
            std::size_t seed = a.data.size();
            for (const T &i : a.data) {
                seed ^= hash<T>()(i) + (size_t)
                        0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

}


template<typename T>
class Array2D {
public:
    unsigned depth;
    unsigned iDsize;

    std::vector<T> data;

    Array2D() {

    }

    Array2D(unsigned iDsize, unsigned depth) noexcept : iDsize(iDsize), depth(depth), data(iDsize * depth) {}


    T &get(unsigned id, unsigned k) noexcept {
        return data[depth * (id) + k];
    }
};


using AbstractFeature   = SvgAbstractFeature;
//using AbstractFeature   = Matrix<Cell>;
//using AbstractFeature   = Matrix<unsigned>;
#endif