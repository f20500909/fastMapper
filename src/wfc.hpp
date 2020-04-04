#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <limits>
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

    WFC(Data<int, AbstractFeature> *data) noexcept : data(data), wave(data), propagator(data) {}

//     运行算法，成功的话并返回一个结果
    void run() noexcept {
        while (true) {
            // 定义未定义的网格值
            ObserveStatus result = observe();
            // 检查算法是否结束
            if (result == success) {
                data->showResult(wave_to_output());
//                data->showResult2(wave);
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
        // 得到具有最低熵的wave_id
        int wave_min_id = wave.get_min_entropy();




//        if (argmin == -1) {
//            observed = new int[FMX * FMY];
//            for (int i = 0; i <wave_size ; i++)
//                for (int j = 0; j < T; j++)
////                    if ([i][t]) { observed[i] = t; break; }
//                    if (this->get(i, j)) {
//                        observed[i] = j;
//                        break;
//                    }
//
//            return true;
//        }

        // 检查冲突，返回failure
        if (wave_min_id == failure || wave_min_id == success) {
            return static_cast<ObserveStatus>(wave_min_id);
        }
        unsigned sum = wave.get_wave_all_frequency(wave_min_id); //得到此wave 在所有feature中出现的次数的总合


        unsigned chosen_value = wave.get_chosen_value_by_random(wave_min_id,sum);

//      如果k对应的图案在argmin中 并且不是选择的元素
        for (unsigned k = 0; k < data->feature.size(); k++) {
            if (wave.get(wave_min_id, k) && k != chosen_value) {
                //添加到传播队列
                propagator.add_to_propagator(wave_min_id, k);
                //设置成已传播的状态
                wave.set(wave_min_id, k, false);
//                std::cout<<" wave_min_id "<< wave_min_id<<" k "<<k<<" chosen_value "<<chosen_value<<"   "<<data->feature.size()<<std::endl;
            }

        }

        //观察结束  继续进行计算
        return to_continue;
    }
};

#endif // FAST_WFC_WFC_HPP_
