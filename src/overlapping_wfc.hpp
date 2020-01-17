#ifndef FAST_WFC_OVERLAPPING_WFC_HPP_
#define FAST_WFC_OVERLAPPING_WFC_HPP_

#include <vector>
#include <unordered_map>

#include "Matrix.hpp"
#include "wfc.hpp"
#include "Data.hpp"
/**
* Options needed to use the overlapping wfc.
*/
struct OverlappingWFCOptions {
    unsigned out_height;  // The height of the output in pixels.
    unsigned out_width;   // The width of the output in pixels.
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    unsigned N; // The width and height in pixel of the patterns.

    /**
    * Get the wave height given these options.
    */
    unsigned get_wave_height() const noexcept {
        return out_height - N + 1;
    }

    /**
    * Get the wave width given these options.
    */
    unsigned get_wave_width() const noexcept {
        return out_width - N + 1;
    }
};

/**
* Class generating a new image with the overlapping WFC algorithm.
* 用wfc算法产生一个新的位图
*/
template<typename T>
class Model {

public:
    /**
    * The input image. T is usually a color.
    * 图像的输入，t通常是一个颜色
    */
    Matrix<T> input;

    /**
    * Options needed by the algorithm.
    */
    OverlappingWFCOptions options;

    /**
    * The array of the different patterns extracted from the input.
    * 从输入图案中提取出的不同图案
    */
    std::vector<Matrix<T>> patterns;

    /**
    * The underlying generic WFC algorithm.
    * 底层的一般wfc算法
    */
    WFC wfc;

    /**
    * Constructor initializing the wfc.
    * This constructor is called by the other constructors.
    * This is necessary in order to initialize wfc only once.
    * 构造函数
    */
    Model(Matrix<Color> &input, const OverlappingWFCOptions &options,
                   std::vector<Matrix<T>> &patterns_1, std::vector<double> &patterns_2,
                   const std::vector<std::array<std::vector<unsigned>, 4>> propagator) noexcept
            : input(input), options(options), patterns(patterns_1),
              wfc( patterns_2, propagator, options.get_wave_height(), options.get_wave_width()) {
    }

//    Model(const Matrix<T> &input, const OverlappingWFCOptions &options,
//          std::vector<Matrix<T>> &patterns_1, std::vector<double> &patterns_2,
//          const std::vector<std::array<std::vector<unsigned>, 4>> propagator) noexcept
//            : input(input), options(options), patterns(patterns_1),
//              wfc( patterns_2, propagator, options.get_wave_height(), options.get_wave_width()) {
//    }

    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    Matrix<T> to_image(const Matrix<unsigned> &output_patterns) const noexcept {
        Matrix<T> output = Matrix<T>(options.out_height, options.out_width);


        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            for (unsigned x = 0; x < options.get_wave_width(); x++) {
                output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            const Matrix<T> &pattern =
                    patterns[output_patterns.get(y, options.get_wave_width() - 1)];
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(y, options.get_wave_width() - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < options.get_wave_width(); x++) {
            const Matrix<T> &pattern =
                    patterns[output_patterns.get(options.get_wave_height() - 1, x)];
            for (unsigned dy = 1; dy < options.N; dy++) {
                output.get(options.get_wave_height() - 1 + dy, x) =
                        pattern.get(dy, 0);
            }
        }
        const Matrix<T> &pattern = patterns[output_patterns.get(
                options.get_wave_height() - 1, options.get_wave_width() - 1)];
        for (unsigned dy = 1; dy < options.N; dy++) {
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(options.get_wave_height() - 1 + dy,
                           options.get_wave_width() - 1 + dx) = pattern.get(dy, dx);
            }
        }
        return output;
    }

public:

    /**
    * Run the WFC algorithm, and return the result if the algorithm succeeded.
    * 运行wfc算法，如果成功返回结果
    */
    std::optional<Matrix<T>> run() noexcept {
        std::optional<Matrix<unsigned>> result = wfc.run();
        if (result.has_value()) {
            return to_image(*result);
        }
        return std::nullopt;
    }
};

#endif // FAST_WFC_WFC_HPP_
