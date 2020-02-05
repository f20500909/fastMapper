#ifndef FAST_WFC_OVERLAPPING_WFC_HPP_
#define FAST_WFC_OVERLAPPING_WFC_HPP_

#include <vector>
#include <unordered_map>

#include "array2D.hpp"
#include "wfc.hpp"

/**
* Options needed to use the overlapping wfc.
*/
struct Options {
    bool periodic_input;  // True if the input is toric.
    bool periodic_output; // True if the output is toric.
    unsigned out_height;  // The height of the output in pixels.
    unsigned out_width;   // The width of the output in pixels.
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    bool ground;       // True if the ground needs to be set (see init_ground).
    unsigned N; // The width and height in pixel of the patterns.

    /**
    * Get the wave height given these options.
    */
    unsigned get_wave_height() const noexcept {
        return periodic_output ? out_height : out_height - N + 1;
    }

    /**
    * Get the wave width given these options.
    */
    unsigned get_wave_width() const noexcept {
        return periodic_output ? out_width : out_width - N + 1;
    }
};

/**
* Class generating a new image with the overlapping WFC algorithm.
* 用wfc算法产生一个新的位图
*/
template<typename T>
class OverlappingWFC {

private:
    /**
    * The input image. T is usually a color.
    * 图像的输入，t通常是一个颜色
    */
    Array2D<T> input;

    /**
    * Options needed by the algorithm.
    */
    Options options;

    /**
    * The array of the different patterns extracted from the input.
    * 从输入图案中提取出的不同图案
    */
    std::vector<Array2D<T>> patterns;

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
    OverlappingWFC(const Array2D<T> &input, const Options &options, const int &seed,
                   const std::pair<std::vector<Array2D<T>>, std::vector<double>> &patterns,
                   const std::vector<std::array<std::vector<unsigned>, 4>>
                   &propagator) noexcept
            : input(input), options(options), patterns(patterns.first),
              wfc(options.periodic_output, seed, patterns.second, propagator, options.get_wave_height(),
                  options.get_wave_width()) {
        // If necessary, the ground is set.
        if (options.ground) {
            init_ground(wfc, input, patterns.first, options);
        }
    }

    /**
    * Constructor used only to call the other constructor with more computed parameters.
    * 构造函数，当有更多参数时调用其他构造函数
    */
    OverlappingWFC(const Array2D<T> &input, const Options &options, const int &seed,
                   const std::pair<std::vector<Array2D<T>>, std::vector<double>>
                   &patterns) noexcept
            : OverlappingWFC(input, options, seed, patterns, generate_compatible(patterns.first)) {}

    /**
    * Init the ground of the output image.
    * The lowest middle pattern is used as a floor (and ceiling when the input is
    * toric) and is placed at the lowest possible pattern position in the output
    * image, on all its width. The pattern cannot be used at any other place in
    * the output image.
    * 初始化输出图片的地面
    * 最底层的图案被用于作为地板
    * 被置于输出图中最底层，在其所有宽度上
    * 图案不能再用于输出中的其他部分
    */
    static void init_ground(WFC &wfc, const Array2D<T> &input, const std::vector<Array2D<T>> &patterns,
                            const Options &options) noexcept {
        unsigned ground_pattern_id =
                get_ground_pattern_id(input, patterns, options);

        // 置图案于底层
        for (unsigned j = 0; j < options.get_wave_width(); j++) {
            for (unsigned p = 0; p < patterns.size(); p++) {
                if (ground_pattern_id != p) {
                    wfc.remove_wave_pattern(options.get_wave_height() - 1, j, p);
                }
            }
        }

        // 移除其他地方的图案
        for (unsigned i = 0; i < options.get_wave_height() - 1; i++) {
            for (unsigned j = 0; j < options.get_wave_width(); j++) {
                wfc.remove_wave_pattern(i, j, ground_pattern_id);
            }
        }

        // Propagate the information with wfc.
        // 传递更新信息
        wfc.propagate();
    }

    /**
    * Return the id of the lowest middle pattern.
    * 返回最底部图案的id
    */
    static unsigned
    get_ground_pattern_id(const Array2D<T> &input,
                          const std::vector<Array2D<T>> &patterns,
                          const Options &options) noexcept {
        // Get the pattern.
        // 获得图案
        Array2D<T> ground_pattern =
                input.get_sub_array(input.height - 1, input.width / 2,
                                    options.N, options.N);

        // Retrieve the id of the pattern.
        // 遍历，取得id
        for (unsigned i = 0; i < patterns.size(); i++) {
            if (ground_pattern == patterns[i]) {
                return i;
            }
        }

        // The pattern exists.
        // 防止图案不存在
        assert(false);
        return 0;
    }

    /**
    * Return the list of patterns, as well as their probabilities of apparition.
    * 返回图案列表，以及它出现的概率
    */
    static std::pair<std::vector<Array2D<T>>, std::vector<double>>
    get_patterns(const Array2D<T> &input, const Options &options) noexcept {
        std::unordered_map<Array2D<T>, unsigned> patterns_id;
        std::vector<Array2D<T>> patterns;

        // The number of time a pattern is seen in the input image.
        // 一个图案在输入中出现的次数
        std::vector<double> patterns_frequency;

        std::vector<Array2D<T>> symmetries(8, Array2D<T>(options.N, options.N));
        unsigned max_i = options.periodic_input ? input.height : input.height - options.N + 1;
        unsigned max_j = options.periodic_input ? input.width : input.width - options.N + 1;

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

        return {patterns, patterns_frequency};
    }

    /**
    * Return true if the pattern1 is compatible with pattern2
    * when pattern2 is at a distance (dy,dx) from pattern1.
    * 当两个图案距离dy，dx时检测是否匹配
    */
    static bool agrees(const Array2D<T> &pattern1, const Array2D<T> &pattern2,
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
    generate_compatible(const std::vector<Array2D<T>> &patterns) noexcept {
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
    Array2D<T> to_image(const Array2D<unsigned> &output_patterns) const noexcept {
        Array2D<T> output = Array2D<T>(options.out_height, options.out_width);

        if (options.periodic_output) {
            for (unsigned y = 0; y < options.get_wave_height(); y++) {
                for (unsigned x = 0; x < options.get_wave_width(); x++) {
                    output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
                }
            }
        } else {
            for (unsigned y = 0; y < options.get_wave_height(); y++) {
                for (unsigned x = 0; x < options.get_wave_width(); x++) {
                    output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
                }
            }
            for (unsigned y = 0; y < options.get_wave_height(); y++) {
                const Array2D<T> &pattern =
                        patterns[output_patterns.get(y, options.get_wave_width() - 1)];
                for (unsigned dx = 1; dx < options.N; dx++) {
                    output.get(y, options.get_wave_width() - 1 + dx) = pattern.get(0, dx);
                }
            }
            for (unsigned x = 0; x < options.get_wave_width(); x++) {
                const Array2D<T> &pattern =
                        patterns[output_patterns.get(options.get_wave_height() - 1, x)];
                for (unsigned dy = 1; dy < options.N; dy++) {
                    output.get(options.get_wave_height() - 1 + dy, x) =
                            pattern.get(dy, 0);
                }
            }
            const Array2D<T> &pattern = patterns[output_patterns.get(
                    options.get_wave_height() - 1, options.get_wave_width() - 1)];
            for (unsigned dy = 1; dy < options.N; dy++) {
                for (unsigned dx = 1; dx < options.N; dx++) {
                    output.get(options.get_wave_height() - 1 + dy,
                               options.get_wave_width() - 1 + dx) = pattern.get(dy, dx);
                }
            }
        }

        return output;
    }

public:
    /**
    * The constructor used by the user.
    * 用户可调用的构造函数
    */
    OverlappingWFC(const Array2D<T> &input, const Options &options, int seed) noexcept
            : OverlappingWFC(input, options, seed, get_patterns(input, options)) {}

    /**
    * Run the WFC algorithm, and return the result if the algorithm succeeded.
    * 运行wfc算法，如果成功返回结果
    */
    std::optional<Array2D<T>> run() noexcept {
        std::optional<Array2D<unsigned>> result = wfc.run();
        if (result.has_value()) {
            return to_image(*result);
        }
        return std::nullopt;
    }
};

#endif // FAST_WFC_WFC_HPP_
