#ifndef FAST_WFC_WAVE_HPP_
#define FAST_WFC_WAVE_HPP_

#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

#include "data.hpp"

class Wave {
public:
    Wave(Data<int, AbstractFeature> *data)
            : plogp(unit::get_plogp(data->features_frequency)),
              wave_size(conf->wave_size), data(data) {
        init_map();
        init_entropy();
    }

    const bool get(unsigned wave_id, unsigned fea_id) const {
        long long key = data->getKey(wave_id, fea_id);
        auto iter = wave_map.find(key);
        if (iter != wave_map.end()) {
            return iter->second;
        } else {
            return false;
        }
    }


    void set(unsigned wave_id, unsigned fea_id, bool status) noexcept {
        bool old_value = this->get(wave_id, fea_id);
        if (old_value == status) return;

        wave_map[data->getKey(wave_id, fea_id)] = status;

        entropy_sum_vec[wave_id] -= plogp[fea_id];
        float& x= frequency_sum_vec[wave_id];

        x -= data->features_frequency[fea_id];

        frequency_num_vec[wave_id]--;
        entropy_vec[wave_id] = log(x) - entropy_sum_vec[wave_id] / x;
    }

    inline unsigned get_wave_frequency(unsigned  wave_id){
        return frequency_num_vec[wave_id];
    }

    inline unsigned get_entropy(unsigned  wave_id){
        return entropy_vec[wave_id];

    }

    const unsigned get_features_frequency(unsigned wave_id, unsigned i) const {
        return this->get(wave_id, i) ? data->features_frequency[i] : 0;
    }

    const unsigned get_wave_all_frequency(unsigned wave_id) const {
        // 遍历所有特征  根据分布结构选择一个元素
        unsigned s = 0;
        for (unsigned k = 0; k < data->feature.size(); k++) {
            // 如果图案存在 就取频次 否则就是0  注意 这里是取频次 不是频率
            s += this->get_features_frequency(wave_id, k);
        }
        return s;
    }


    // 随机数逐步减小 小于0时中断
    const unsigned get_chosen_value_by_random(unsigned wave_id, unsigned sum) const {
        unsigned chosen_value = 0;
        float random_value = unit::getRand(0, sum);  //随机生成一个noise

        while (chosen_value < data->feature.size() && random_value > 0) {
            random_value -= this->get_features_frequency(wave_id, chosen_value);
            chosen_value++;
        }

        if (chosen_value != 0) chosen_value--;
        return chosen_value;
    }


private:
    const unsigned wave_size;

    const std::vector<float> plogp;

    unordered_map<long long, bool> wave_map;

    Data<int, AbstractFeature> *data;

    std::vector<float> entropy_sum_vec; // The sum of p'(fea) * log(p'(fea)).
    std::vector<float> frequency_sum_vec;       // The features_frequency_sum of p'(fea).
    std::vector<unsigned> frequency_num_vec; // The number of feature present
    std::vector<float> entropy_vec;       // The entropy of the cell



    void init_entropy() {
        float entropy_sum = 0;
        float frequency_sum = 0;

        for (unsigned i = 0; i < data->feature.size(); i++) {
            entropy_sum += plogp[i];        // 所有熵的和
            frequency_sum += data->features_frequency[i];      //频率和
        }
        float log_sum = log(frequency_sum);

        entropy_sum_vec = std::vector<float>(wave_size, entropy_sum);
        frequency_sum_vec = std::vector<float>(wave_size, frequency_sum);
        frequency_num_vec = std::vector<unsigned>(wave_size, data->feature.size());
        entropy_vec = std::vector<float>(wave_size, log_sum - entropy_sum / frequency_sum);
    }

    void init_map() {
        for (unsigned i = 0; i < conf->wave_size; i++) {
            for (unsigned j = 0; j < data->feature.size(); j++) {
                wave_map[data->getKey(i, j)] = true;
            }
        }
    }

};

#endif // FAST_WFC_WAVE_HPP_

