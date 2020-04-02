#ifndef FAST_WFC_WAVE_HPP_
#define FAST_WFC_WAVE_HPP_

#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <unordered_map>

#include "data.hpp"

class Wave {
private:

    const std::vector<float> plogp_features_frequency;

    const float half_min_plogp;

    bool is_impossible;

    unordered_map<long long, bool> wave_map;

    Data<int, AbstractFeature> *data;

    std::vector<float> p_log_p_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<float> features_frequency_sum;       // The features_frequency_sum of p'(pattern).
    std::vector<float> log_sum;   // The log of sum.
    std::vector<unsigned> features_number_vec; // The number of feature present
    std::vector<float> entropy_vec;       // The entropy of the cell.c

    const unsigned wave_size;

public:

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
    * Initialize the wave with every cell being able to have every pattern.
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
    * Return true if pattern can be placed in cell index.
    * 返回true如果图案能放入cell
    */
    const bool get(unsigned index, unsigned pattern) const {

        long long key = data->getKey(index, pattern);
        auto iter = wave_map.find(key);
        if (iter != wave_map.end()) {
            return iter->second;
        } else {
            return false;
        }
    }

    const float get_features_frequency(unsigned feature_id, unsigned i) const {
        return this->get(feature_id, i) ? data->features_frequency[i] : 0;
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

        // 判定终止条件
        if (features_number_vec[fea_1] == 0){
            is_impossible = true;
        }
    }

    /**
    * 返回不为0的最小熵的索引
    * 如果中间有contradiction在wave中，则返回-2
    * 如果所有cell都被定义，返回-1
    */
    const int get_min_entropy() const noexcept {
        if (is_impossible) {
            return failure;
        }

        float min = std::numeric_limits<float>::infinity();
        int argmin = success;

        for (unsigned i = 0; i < wave_size; i++) {

            if (features_number_vec[i] == 1) {
                // If the cell is decided, we do not compute the entropy (which is equal to 0).
                // 如果cell被决定，我们不用再计算信息熵
                continue;
            }

            //拿到当前的熵
            float entropy = entropy_vec[i];

            // We first check if the entropy is less than the minimum.
            // This is important to reduce noise computation (which is not* negligible).
            //检查熵是否比最小值小  如果小才更新argmin 不过效果我测试的真的不太明显
            if (entropy <= min) {
                // Then, we add noise to decide randomly which will be chosen.
                // noise is smaller than the smallest p * log(p), so the minimum entropy
                // will always be chosen.
                float noise = unit::getRand(float(0), abs(half_min_plogp));  //随机生成一个noise
                if (entropy + noise < min) {
                    min = std::min(entropy + noise, min);
                    argmin = i;
                }
            }
        }

        return argmin;
    }

    inline unsigned size() const noexcept { return wave_size; };

};

#endif // FAST_WFC_WAVE_HPP_

