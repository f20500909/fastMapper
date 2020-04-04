#ifndef FAST_WFC_WAVE_HPP_
#define FAST_WFC_WAVE_HPP_

#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

#include "data.hpp"

class Wave {
public:

    const std::vector<unsigned> plogp_features_frequency;

    const float half_min_plogp;

    bool is_impossible;

    unordered_map<long long, bool> wave_map;

    Data<int, AbstractFeature> *data;

    std::vector<float> p_log_p_sum; // The sum of p'(fea) * log(p'(fea)).
    std::vector<float> features_frequency_sum;       // The features_frequency_sum of p'(fea).
    std::vector<float> log_sum;   // The log of sum.
    std::vector<unsigned> features_number_vec; // The number of feature present
    std::vector<float> entropy_vec;       // The entropy of the cell.c

    const unsigned wave_size;


    void init_map() {
        for (unsigned i = 0; i < data->options.wave_size; i++) {

            for (unsigned j = 0; j < data->feature.size(); j++) {
                wave_map[data->getKey(i, j)] = true;
            }
        }
    }

    void init_entropy() {
        float base_entropy = 0;
        float base_sum = 0;

        for (unsigned i = 0; i < data->feature.size(); i++) {
            base_entropy += plogp_features_frequency[i];// 累加所有  p log(p) 的和
            base_sum += data->features_frequency[i];//     频率的和
        }
        float base_log_sum = log(base_sum);

        p_log_p_sum = std::vector<float>(wave_size, base_entropy);
        features_frequency_sum = std::vector<float>(wave_size, base_sum);
        log_sum = std::vector<float>(wave_size, base_log_sum);
        features_number_vec = std::vector<unsigned>(wave_size, data->feature.size());
        entropy_vec = std::vector<float>(wave_size, base_log_sum - base_entropy / base_sum);
    }


    /**
    * Initialize the wave with every cell being able to have every fea.
    * 初始化wave中每个cell
    */
    Wave(Data<int, AbstractFeature> *data) noexcept
            : plogp_features_frequency(unit::get_plogp(data->features_frequency)),
              half_min_plogp(unit::get_half_min(plogp_features_frequency)),
              wave_size(data->options.wave_size), data(data) {
        init_map();
        init_entropy();
        is_impossible = false;
    }

    /**
    * Return true if fea can be placed in cell index.
    * 返回true如果图案能放入cell
    */
    const bool get(unsigned index, unsigned fea_id) const {

        long long key = data->getKey(index, fea_id);
        auto iter = wave_map.find(key);
        if (iter != wave_map.end()) {
            return iter->second;
        } else {
            return false;
        }
    }

    const unsigned get_features_frequency(unsigned feature_id, unsigned i) const {
        return this->get(feature_id, i) ? data->features_frequency[i] : 0;
    }

    const unsigned get_wave_all_frequency(unsigned wave_id) const {
        // 遍历所有特征  根据分布结构选择一个元素
        unsigned s = 0;
        for (unsigned k = 0; k < data->feature.size(); k++) {
            // 如果图案存在 就取频次 否则就是0  注意 这里是取频次 不是频率
            s += this->get_features_frequency(wave_id, k);
        }
        return  s;
    }


    // 随机数逐步减小 小于0时中断
    const unsigned get_chosen_value_by_random(unsigned wave_id,unsigned sum) const {
        unsigned chosen_value = 0;
        float random_value = unit::getRand(0, sum);  //随机生成一个noise

        while (chosen_value < data->feature.size() && random_value > 0) {
            random_value -= this->get_features_frequency(wave_id, chosen_value);
            chosen_value++;
        }

        if (chosen_value != 0) chosen_value--;
        return chosen_value;
    }


    void set(unsigned fea_1, unsigned fea_2, bool value) noexcept {
        bool old_value = this->get(fea_1, fea_2);
        if (old_value == value) return;

        // Otherwise, the memoisation should be updated.
        wave_map[data->getKey(fea_1, fea_2)] = value;

        p_log_p_sum[fea_1] -= plogp_features_frequency[fea_2];
        features_frequency_sum[fea_1] -= data->features_frequency[fea_2];
        log_sum[fea_1] = log(features_frequency_sum[fea_1]);
        features_number_vec[fea_1]--;
        entropy_vec[fea_1] = log_sum[fea_1] - p_log_p_sum[fea_1] / features_frequency_sum[fea_1];

    }



    inline unsigned size() const noexcept { return wave_size; };

};

#endif // FAST_WFC_WAVE_HPP_

