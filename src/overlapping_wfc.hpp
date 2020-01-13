#ifndef FAST_WFC_OVERLAPPING_WFC_HPP_
#define FAST_WFC_OVERLAPPING_WFC_HPP_

#include <vector>
#include <unordered_map>

#include "Data.hpp"
#include "wfc.hpp"

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
class OverlappingWFC {

public:
    /**
    * The input image. T is usually a color.
    * 图像的输入，t通常是一个颜色
    */
    Data<T> input;

    /**
    * Options needed by the algorithm.
    */
    OverlappingWFCOptions options;

    /**
    * The array of the different patterns extracted from the input.
    * 从输入图案中提取出的不同图案
    */
    std::vector<Data<T>> patterns;

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
    OverlappingWFC(const Data<T> &input, const OverlappingWFCOptions &options, const int &seed,
                   std::vector<Data<T>> &patterns_1, std::vector<double> &patterns_2,
                   const std::vector<std::array<std::vector<unsigned>, 4>> &propagator) noexcept
            : input(input), options(options), patterns(patterns_1),
              wfc(seed, patterns_2, propagator, options.get_wave_height(), options.get_wave_width()) {
    }

    /**
    * Return true if the pattern1 is compatible with pattern2
    * when pattern2 is at a distance (dy,dx) from pattern1.
    * 当两个图案距离dy，dx时检测是否匹配
    */
    static bool agrees(const Data<T> &pattern1, const Data<T> &pattern2,
                       int dy, int dx) noexcept {
        unsigned xmin = dx < 0 ? 0 : dx;
        unsigned xmax = dx < 0 ? dx + pattern2.width : pattern1.width;
        unsigned ymin = dy < 0 ? 0 : dy;
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
    * Precompute the function agrees(pattern1, pattern2, dy, dx).
    * If agrees(pattern1, pattern2, dy, dx), then compatible[pattern1][direction]
    * contains pattern2, where direction is the direction defined by (dy, dx) (see direction.hpp).
    * 先计算是否匹配
    * 如果匹配，则合并
    */
    static std::vector<std::array<std::vector<unsigned>, 4>>
    generate_compatible(const std::vector<Data<T>> &patterns) noexcept {
        std::vector<std::array<std::vector<unsigned>, 4>> compatible =
                std::vector<std::array<std::vector<unsigned>, 4>>(patterns.size());

        // Iterate on every dy, dx, pattern1 and pattern2
        for (unsigned pattern1 = 0; pattern1 < patterns.size(); pattern1++) {
            for (unsigned direction = 0; direction < 4; direction++) {
                for (unsigned pattern2 = 0; pattern2 < patterns.size(); pattern2++) {
                    if (agrees(patterns[pattern1], patterns[pattern2], directions_y[direction],
                               directions_x[direction])) {
                        compatible[pattern1][direction].push_back(pattern2);
                    }
                }
            }
        }

        return compatible;
    }

    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    Data<T> to_image(const Data<unsigned> &output_patterns) const noexcept {
        Data<T> output = Data<T>(options.out_height, options.out_width);


        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            for (unsigned x = 0; x < options.get_wave_width(); x++) {
                output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            const Data<T> &pattern =
                    patterns[output_patterns.get(y, options.get_wave_width() - 1)];
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(y, options.get_wave_width() - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < options.get_wave_width(); x++) {
            const Data<T> &pattern =
                    patterns[output_patterns.get(options.get_wave_height() - 1, x)];
            for (unsigned dy = 1; dy < options.N; dy++) {
                output.get(options.get_wave_height() - 1 + dy, x) =
                        pattern.get(dy, 0);
            }
        }
        const Data<T> &pattern = patterns[output_patterns.get(
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
    std::optional<Data<T>> run() noexcept {
        std::optional<Data<unsigned>> result = wfc.run();
        if (result.has_value()) {
            return to_image(*result);
        }
        return std::nullopt;
    }
};

#endif // FAST_WFC_WFC_HPP_
