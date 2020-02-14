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
#include "declare.hpp"
#include "base.hpp"
#include "imageModel.hpp"

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
    std::vector<Matrix<Cell>> patterns;

    /**
     * The wave, indicating which patterns can be put in which cell.
     * wave，表示哪个图案应该被填入哪个格中
     */
    Wave wave;

    /**
     * The distribution of the patterns as given in input.
     * 输入中给的分布模式
     */
    const std::vector<double> patterns_frequency;

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
    WFC(Data<int> *_data, const Options &options) noexcept
            : data(_data), options(options), patterns(_data->patterns), gen(rand()),
              wave(options, _data->patterns_frequency),
              patterns_frequency(_data->patterns_frequency), nb_patterns(_data->propagator.size()),
              propagator(wave.height, wave.width, _data->propagator, options) {
    }

    Data<int> *data;

//     运行算法，成功的话并返回一个结果
    void run() noexcept {

        while (true) {
            // Define the value of an undefined cell.
            // 定义未定义的网格值
            ObserveStatus result = observe();
            // 检查算法是否结束
            assert(result != failure);
            if (result == success) {
                Matrix<unsigned> res = wave_to_output();

                data->showResult(res);
//                Img<int>* t = static_cast<Img<int>*>(data);
//                t->showResult(res);
//                static_cast<Img<int>*>(data)->showResult(res);
//                (Img<int>*)(data)->showResult(res);

                return;
            }
            // 传递信息
            propagator.propagate(wave);
        }
    }

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
            s += wave.get(argmin, k) ? patterns_frequency[k] : 0;
        }

        std::uniform_real_distribution<> dis(0, s);
        double random_value = dis(gen);
        unsigned chosen_value = nb_patterns - 1;

        //小于0时中断
        for (unsigned k = 0; k < nb_patterns; k++) {
            random_value -= wave.get(argmin, k) ? patterns_frequency[k] : 0;
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

    Options options;
};

#endif // FAST_WFC_WFC_HPP_
