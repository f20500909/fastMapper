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

using namespace std;

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

//需要弱化方向的概念 让方向与模型适配
class DirectionSet {
public:

    DirectionSet(int directionNumbers) : increment_angle(360.0 / directionNumbers) {


        //初始化方向的方向
        initDirectionWithAngle();
    }

    //初始化角度
    void initDirectionWithAngle() {
        _data_angle = std::vector<std::pair<float, float>>(_data.size(), {0, 0});
        _data_opp_angle = std::vector<std::pair<float, float>>(_data.size(), {0, 0});
        for (unsigned i = 0; i < _data.size(); i++) {
            _data_angle[i] = {i * increment_angle, (i + 1) * increment_angle};
        }

        for (unsigned i = 0; i < _data.size(); i++) {
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


    unsigned get_opposite_direction(unsigned id) noexcept {
        return ((_data.size() >> 1) + id) % _data.size();
    }

    unsigned getMaxNumber() {
        return _data.size();
    }



    unsigned movePatternByDirection(unsigned dId, unsigned wave_width) {
       std::pair<int,int>  direction = _data[dId];
        return direction.first + direction.second * wave_width;
    }


    int get_angle_direction_id(float angle, bool is_opp) {
        return std::min(angle / increment_angle, static_cast<float>(_data.size() - 1));
    }

    const float increment_angle;  // 360除以 8   每个方向即是45度
};

template<typename T>
class Matrix {

public:

    unsigned height;
    unsigned width;

    std::vector<T> data;

    Matrix() {

    };

    std::vector<int> direction_fea_id_vec;


    Matrix(unsigned height, unsigned width) noexcept : height(height), width(width), data(width * height) {}


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
//        assert(i < height && j < width);
        return data[j + i * width];
    }




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


    Matrix<T> get_sub_array(unsigned y, unsigned x, unsigned sub_width,
                            unsigned sub_height) const noexcept {
        Matrix<T> sub_array_2d = Matrix<T>(sub_width, sub_height);
        for (unsigned ki = 0; ki < sub_height; ki++) {
            for (unsigned kj = 0; kj < sub_width; kj++) {
                sub_array_2d.get(ki, kj) = this->get((y + ki) % height, (x + kj) % width);
            }
        }

        return sub_array_2d;
    }

    Matrix<T> &operator=(const Matrix<T> &a) noexcept {
        height = a.height;
        width = a.width;
        data = a.data;
        return *this;
    }


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



//using AbstractFeature   = SvgAbstractFeature;
using AbstractFeature   = Matrix<unsigned>;
#endif