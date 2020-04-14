#ifndef SRC_DECLARE_HPP
#define SRC_DECLARE_HPP

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION

#include "include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image_write.h"

#include "unit.hpp"
#include "bitMap.hpp"
#include "include/cmdline.h"

using namespace std;

template<typename T>
class Matrix;

template<class T, class AbstractFeature>
class Data;

class SvgAbstractFeature;

enum ObserveStatus {
    success = -10, // wfc完成并取得成功
    failure = -9, // wfc完成并失败
    to_continue = -8, // wfc没有完成

    amount_flag,
};


class Config {
public:
    unsigned out_height;
    unsigned out_width;
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    unsigned N; // The width and height in pixel of the feature.
    unsigned channels; // The width and height in pixel of the feature.
    int log;
    std::string input_data; // The width and height in pixel of the feature.
    std::string output_data; // The width and height in pixel of the feature.
    std::string type;        // 模式

    unsigned wave_height;  // The height of the output in pixels.
    unsigned wave_width;   // The width of the output in pixels.

    unsigned wave_size;   // The width of the output in pixels.

    Config(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels, int log,
           string input_data, std::string output_data, std::string type) :
            out_height(out_height),
            out_width(out_width),
            symmetry(symmetry),
            N(N),
            channels(channels),
            log(log),
            input_data(std::move(input_data)),
            output_data(std::move(output_data)),
            type(std::move(type)),
            wave_height(out_height - N + 1),
            wave_width(out_width - N + 1),
            wave_size(wave_height * wave_width) {
        showLog();
    }

    Config() {

    }

    void showLog() {

        cout << "==============  conf  ================" << endl
             << "height                   : " << this->out_height << endl
             << "width                    : " << this->out_width << endl
             << "symmetry                 : " << this->symmetry << endl
             << "N                        : " << this->N << endl
             << "channels                 : " << this->channels << endl
             << "log                      : " << this->log << endl
             << "input_data               : " << this->input_data << endl
             << "output_data              : " << this->output_data << endl
             << "type                     : " << this->type << endl
             << "==================================" << endl;
    }

    static Config *op;

    static Config *getOp();
};


Config *Config::op = new Config;

Config *Config::getOp() { return op; };
Config *conf = Config::getOp();


//需要弱化方向的概念 让方向与模型适配
class DirectionSet {
public:

    DirectionSet(int directionNumbers) : increment_angle(180.0 / directionNumbers) {
        //初始化方向的方向
        initDirectionWithAngle();
    }

    //初始化角度
    void initDirectionWithAngle() {
//        _data_angle = std::vector<std::pair<float, float>>(_direct.size(), {0, 0});
//        _data_opp_angle = std::vector<std::pair<float, float>>(_direct.size(), {0, 0});
//        for (unsigned i = 0; i < _direct.size(); i++) {
//            _data_angle[i] = {i * increment_angle, (i + 1) * increment_angle};
//        }
//
//        for (unsigned i = 0; i < _direct.size(); i++) {
//            unsigned index = get_opposite_direction(i);
//            _data_opp_angle[i] = _data_angle[index];
//        }
    }


    int getX(unsigned directionId) {
        return this->getDirect(directionId).first;
    }

    int getY(unsigned directionId) {
        return this->getDirect(directionId).second;
    }

    unsigned get_opposite_direction(unsigned fea_id,unsigned id) noexcept {
        return ((_direct.size() >> 1) + id) % _direct.size();
    }

    unsigned getMaxNumber() {
        return _direct.size();
    }

    int movePatternByDirection(unsigned wave_id, unsigned dId, unsigned wave_width) {
        std::pair<int, int> &direction = _direct[dId];
        return wave_id + direction.first + direction.second * wave_width;
    }

    std::pair<int, int> &getDirect(unsigned dId) {
        return this->_direct[dId];
    }

    std::vector<std::pair<int, int>> &getDirect() {
        return this->_direct;
    }


//    int get_angle_direction_id(float angle, bool is_opp) {
//        return std::min(angle / increment_angle, static_cast<float>(_direct.size() - 1));
//    }

private:

    std::vector<std::pair<int, int>> _direct = {{0,  -1},
                                                {-1, 0},
                                                {1,  0},
                                                {0,  1}};

    std::vector<std::pair<float, float>> _data_angle; // 角度方向数组   <角度上限，角度下限>  依次规律递增

    std::vector<std::pair<float, float>> _data_opp_angle; // 角度反方向数组   <角度上限，角度下限>  依次规律递增

    const float increment_angle;  // 360除以 8   每个方向即是45度
};

template<typename T>
class Matrix {
public:

    Matrix() {

    };

    std::vector<int> direction_fea_id_vec;

    Matrix(unsigned height, unsigned width) noexcept : height(height), width(width), data(width * height) {}


    Matrix(unsigned height, unsigned width, T value) noexcept
            : height(height), width(width), data(width * height, value) {}

    const unsigned getHeight() const {
        return this->height;
    }

    const unsigned getWidth() const {
        return this->width;
    }

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

    Matrix<T> reflected() const noexcept {
        Matrix<T> result = Matrix<T>(width, height);
        for (unsigned y = 0; y < height; y++) {
            for (unsigned x = 0; x < width; x++) {
                result.get(y, x) = get(y, width - 1 - x);
            }
        }
        return result;
    }

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

    std::vector<T> data;
private:
    unsigned height;
    unsigned width;
};

namespace std {
    template<typename T>
    class hash<Matrix<T>> {
    public:
        size_t operator()(const Matrix<T> &a) const noexcept {
            std::size_t seed = a.data.size();
            for (const T &i : a.data) {
                //0x9e3779b9 是一个黄金分割数
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