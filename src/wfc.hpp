#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <cmath>
#include <limits>
#include <random>
#include <unordered_map>

#include "propagator.hpp"

/**
 * Class containing the generic WFC algorithm.
 */
class WFC {
private:
    Data<int, AbstractFeature> *data;

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
                break;
            }
            // 传递信息
            propagator.propagate(wave);
        }
    }

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

//        // 随机数逐步减小 小于0时中断
        unsigned chosen_value = 0;
        float random_value = unit::getRand(float(0), s);  //随机生成一个noise

        while (chosen_value < data->feature.size() && random_value > 0) {
            random_value -= wave.get_features_frequency(argmin, chosen_value);
            chosen_value++;
        }

        if(chosen_value!=0)chosen_value--;


        // 根据图案定义网格
        for (unsigned k = 0; k < data->feature.size(); k++) {
            /* 判定生效的情况：
             如果k对应的图案在argmin中 并且不是选择的元素
             */
            if (wave.get(argmin, k) && !(k == chosen_value)) {
                propagator.add_to_propagator(argmin, k);
                wave.set(argmin, k, false);
//                std::cout<<" argmin "<< argmin<<" k "<<k<<" chosen_value "<<chosen_value<<"   "<<data->feature.size()<<std::endl;
            }

        }

        return to_continue;
    }
};

#endif // FAST_WFC_WFC_HPP_
