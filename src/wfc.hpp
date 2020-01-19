#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <cmath>
#include <limits>
#include <random>
#include <unordered_map>
#include <optional>

#include "Data.hpp"
#include "Matrix.hpp"
#include "propagator.hpp"
#include "wave.hpp"



/**
 * Class containing the generic WFC algorithm.
 */
class WFC {
private:
    /**
     * The random number generator.
     * 随机种子
     */
    std::minstd_rand gen;

    /**
    * The array of the different patterns extracted from the input.
    * 从输入图案中提取出的不同图案
    */
    std::vector<Matrix<Color>> patterns;

    /**
     * The wave, indicating which patterns can be put in which cell.
     * wave，表示哪个图案应该被填入哪个格中
     */
    Wave wave;

    /**
     * The distribution of the patterns as given in input.
     * 输入中给的分布模式
     */
    const std::vector<double> patterns_frequencies;

    /**
     * The number of distinct patterns.
     * 不同图案的数量
     */
    const unsigned nb_patterns;

    /**
     * The propagator, used to propagate the information in the wave.
     * 传递器，用于传递wave中的信息
     */
    Propagator propagator;

    /**
     * Transform the wave to a valid output (a 2d array of patterns that aren't in contradiction).
     * This function should be used only when all cell of the wave are defined.
     * 将波转换为有效的输出（一个不矛盾的2d阵列）
     * 此函数只有当波的所有格子都被定义
     */
    Matrix<unsigned> wave_to_output() const noexcept {
        Matrix<unsigned> output_patterns(wave.height, wave.width);
        for (unsigned i = 0; i < wave.size; i++) {
            for (unsigned k = 0; k < nb_patterns; k++) {
                if (wave.get(i, k)) {
                    output_patterns.data[i] = k;
                }
            }
        }
        return output_patterns;
    }

public:
    /**
     * Basic constructor initializing the algorithm.
     * 构造函数，初始化
     */
    WFC(const OverlappingWFCOptions &options, std::vector<Matrix<Color>> &patterns,
        std::vector<double> patterns_frequencies, Propagator::PropagatorState propagator,
        unsigned wave_height, unsigned wave_width) noexcept
            : options(options), patterns(patterns), gen(rand()), wave(wave_height, wave_width, patterns_frequencies),
              patterns_frequencies(patterns_frequencies), nb_patterns(propagator.size()),
              propagator(wave.height, wave.width, propagator) {

    }

//     运行算法，成功的话并返回一个结果
    std::optional<Matrix<unsigned>> run() noexcept {
        while (true) {
            // Define the value of an undefined cell.
            // 定义未定义的网格值
            ObserveStatus result = observe();
            // 检查算法是否结束
            if (result == failure) {
                return std::nullopt;
            } else if (result == success) {
                return wave_to_output();
            }
            // 传递信息
            propagator.propagate(wave);
        }
    }

    enum ObserveStatus {
        success, // wfc完成并取得成功
        failure, // wfc完成并失败
        to_continue // wfc没有完成
    };

    /**
     * Define the value of the cell with lowest entropy.
     * 定义具有最低熵的网格值
     */
    ObserveStatus observe() noexcept {
        // 得到具有最低熵的网格
        int argmin = wave.get_min_entropy(gen);
        // 检查冲突，返回failure
        if (argmin == -2) {
            return failure;
        }

        // 如果最低熵是-1，那么算法成功并完成
        if (argmin == -1) {
            wave_to_output();
            return success;
        }

        // 根据分布结构选择一个元素
        double s = 0;
        for (unsigned k = 0; k < nb_patterns; k++) {
            s += wave.get(argmin, k) ? patterns_frequencies[k] : 0;
        }

        std::uniform_real_distribution<> dis(0, s);
        double random_value = dis(gen);
        unsigned chosen_value = nb_patterns - 1;

        //小于0时中断
        for (unsigned k = 0; k < nb_patterns; k++) {
            random_value -= wave.get(argmin, k) ? patterns_frequencies[k] : 0;
            if (random_value <= 0) {
                chosen_value = k;
                break;
            }
        }

        // 根据图案定义网格
        for (unsigned k = 0; k < nb_patterns; k++) {
            if (wave.get(argmin, k) != (k == chosen_value)) {
                propagator.add_to_propagator(argmin / wave.width, argmin % wave.width, k);
                wave.set(argmin, k, false);
            }
        }
        return to_continue;
    }


    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    Matrix<Color> to_image(const Matrix<unsigned> &output_patterns) const noexcept {
        Matrix<Color> output = Matrix<Color>(options.out_height, options.out_width);


        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            for (unsigned x = 0; x < options.get_wave_width(); x++) {
                output.get(y, x) = patterns[output_patterns.get(y, x)].get(0, 0);
            }
        }
        for (unsigned y = 0; y < options.get_wave_height(); y++) {
            const Matrix<Color> &pattern =
                    patterns[output_patterns.get(y, options.get_wave_width() - 1)];
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(y, options.get_wave_width() - 1 + dx) = pattern.get(0, dx);
            }
        }
        for (unsigned x = 0; x < options.get_wave_width(); x++) {
            const Matrix<Color> &pattern =
                    patterns[output_patterns.get(options.get_wave_height() - 1, x)];
            for (unsigned dy = 1; dy < options.N; dy++) {
                output.get(options.get_wave_height() - 1 + dy, x) =
                        pattern.get(dy, 0);
            }
        }
        const Matrix<Color> &pattern = patterns[output_patterns.get(
                options.get_wave_height() - 1, options.get_wave_width() - 1)];
        for (unsigned dy = 1; dy < options.N; dy++) {
            for (unsigned dx = 1; dx < options.N; dx++) {
                output.get(options.get_wave_height() - 1 + dy,
                           options.get_wave_width() - 1 + dx) = pattern.get(dy, dx);
            }
        }
        return output;
    }


    /**
* Options needed by the algorithm.
*/
    OverlappingWFCOptions options;


};

#endif // FAST_WFC_WFC_HPP_
