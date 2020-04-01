#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <cmath>
#include <limits>
#include <random>
#include <unordered_map>
#include <optional>

#include "propagator.hpp"

/**
 * Class containing the generic WFC algorithm.
 */
class WFC {
private:
    Data<int, AbstractFeature> *data;

    /**
     * The random number generator.
     * 随机种子
     */
    std::minstd_rand gen;


    /**
     * The wave, indicating which feature can be put in which cell.
     * wave，表示哪个图案应该被填入哪个格中
     */
    Wave wave;

    /**
     * The propagator, used to propagate the information in the wave.
     * 传递器，用于传递wave中的信息
     */
    Propagator propagator;

    /**
     * Transform the wave to a valid output (a 2d array of feature that aren't in contradiction).
     * This function should be used only when all cell of the wave are defined.
     * 将波转换为有效的输出（一个不矛盾的2d阵列）
     * 此函数只有当波的所有格子都被定义
     */
    Matrix<unsigned> wave_to_output() noexcept {
        Matrix<unsigned> output_features(data->options.wave_height, data->options.wave_width);
        for (unsigned i = 0; i < wave.size(); i++) {
            for (unsigned k = 0; k < data->feature.size(); k++) {
                if (wave.get(i, k)) {
                    output_features.get(i) = k;
                }
            }
        }
        return output_features;
    }

public:
    /**
     * Basic constructor initializing the algorithm.
     * 构造函数，初始化
     */
    WFC(Data<int, AbstractFeature> *data) noexcept
            : data(data), wave(data), propagator(data) {
    }

//     运行算法，成功的话并返回一个结果
    void run() noexcept {
        while (true) {
            // 定义未定义的网格值
            ObserveStatus result = observe();
            // 检查算法是否结束
            if (result == success) {
                data->showResult(wave_to_output());
                return;
            }

            if (result == failure) {
                data->showResult(wave_to_output());
                std::cout << "failure!!!!!!!!!!!!!!" << std::endl;
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
        int argmin = wave.get_min_entropy();

        // 检查冲突，返回failure
        if (argmin == failure || argmin == success) {
            return static_cast<ObserveStatus>(argmin);
        }

        // 遍历所有特征  根据分布结构选择一个元素
        float s = 0;
        for (unsigned k = 0; k < data->feature.size(); k++) {
            // 如果图案存在 就取频次 否则就是0  注意 这里是取频次 不是频率
            s += wave.get_features_frequency(argmin, k);
        }

        float random_value = unit::getRand(float(0), s);  //随机生成一个noise

        unsigned chosen_value = data->feature.size() - 1;

        //小于0时中断 将频率自减少  为什么原作者这里random_value 是double?
        for (unsigned k = 0; k < data->feature.size(); k++) {
            random_value -= wave.get_features_frequency(argmin, k);
            if (random_value <= 0) {
                chosen_value = k;
                break;
            }
        }

        // 根据图案定义网格
        for (unsigned k = 0; k < data->feature.size(); k++) {
            /* 判定生效的情况：
             如果k对应的图案在argmin中 并且不是选择的元素
             如果k对应的团不在argmin中 并且是选择的元素
             加上此判断主要是为了不选择自身
             */
            if (wave.get(argmin, k) != (k == chosen_value)) {
                propagator.add_to_propagator(argmin, k);
                wave.set(argmin, k, false);
            }
        }

        return to_continue;
    }
};

#endif // FAST_WFC_WFC_HPP_
