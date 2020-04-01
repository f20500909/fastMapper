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

    const std::vector<double> plogp_features_frequency;

    const double half_min_plogp;

    bool is_impossible;

    unordered_map<long long, bool> wave_map;

    Data<int, AbstractFeature> *data;

    std::vector<double> p_log_p_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;       // The sum of p'(pattern).
    std::vector<double> log_sum;   // The log of sum.
    std::vector<unsigned> features_number_vec; // The number of feature present
    std::vector<double> entropy_vec;       // The entropy of the cell.c

    const unsigned _size;

public:

    void init_map() {
        for (int i = 0; i < data->options.wave_size; i++) {

            for (int j = 0; j < data->feature.size(); j++) {
                wave_map[data->getKey(i, j)] = true;
            }
        }
    }

    void init_entropy() {
        double base_entropy = 0;
        double base_s = 0;

        for (unsigned i = 0; i < data->feature.size(); i++) {
            base_entropy += plogp_features_frequency[i];// plogp 的和
            base_s += data->features_frequency[i];// 频率的和
        }
        double log_base_s = log(base_s);
        double entropy_base = log_base_s - base_entropy / base_s;

        p_log_p_sum = std::vector<double>(_size, base_entropy);
        sum = std::vector<double>(_size, base_s);
        log_sum = std::vector<double>(_size, log_base_s);
        features_number_vec = std::vector<unsigned>(_size, data->feature.size());
        entropy_vec = std::vector<double>(_size, entropy_base);
    }


    /**
    * Initialize the wave with every cell being able to have every pattern.
    * 初始化wave中每个cell
    */
    Wave(Data<int, AbstractFeature> *data) noexcept
            : plogp_features_frequency(unit::get_plogp(data->features_frequency)),
              half_min_plogp(unit::get_half_min(plogp_features_frequency)),
              _size(data->options.wave_size), data(data) {
        init_map();
        init_entropy();
        is_impossible = false;
    }

    /**
    * Return true if pattern can be placed in cell index.
    * 返回true如果图案能放入cell
    */
    bool get(unsigned index, unsigned pattern) const {

        long long key = data->getKey(index, pattern);
        auto iter = wave_map.find(key);
        if (iter != wave_map.end()) {
            return iter->second;
        } else {
            return false;
        }
    }

    const double get_features_frequency(unsigned feature_id, unsigned i) const {
        return this->get(feature_id, i) ? data->features_frequency[i] : 0;
    }

    void set(unsigned fea_1, unsigned fea_2, bool value) noexcept {
        bool old_value = this->get(fea_1, fea_2);
        // If the value isn't changed, nothing needs to be done.
        if (old_value == value) return;

        // Otherwise, the memoisation should be updated.
        wave_map[data->getKey(fea_1, fea_2)] = value;

        p_log_p_sum[fea_1] -= plogp_features_frequency[fea_2];
        sum[fea_1] -= data->features_frequency[fea_2];
        log_sum[fea_1] = log(sum[fea_1]);
        features_number_vec[fea_1]--;
        entropy_vec[fea_1] = log_sum[fea_1] - p_log_p_sum[fea_1] / sum[fea_1];

        if (features_number_vec[fea_1] == 0) is_impossible = true;
    }

    /**
    * 返回不为0的最小熵的索引
    * 如果中间有contradiction在wave中，则返回-2
    * 如果所有cell都被定义，返回-1
    */
    int get_min_entropy(std::minstd_rand &gen) const noexcept {
        if (is_impossible) {
            return failure;
        }

        std::uniform_real_distribution<> dis(0, abs(half_min_plogp));

        double min = std::numeric_limits<double>::infinity();
        int argmin = success;

        for (int i = 0; i < _size; i++) {
            // If the cell is decided, we do not compute the entropy (which is equal to 0).
            // 如果cell被决定，我们不用再计算信息熵
            double feature_number = features_number_vec[i];
            if (feature_number == 1) {
                continue;
            }

            // Otherwise, we take the memoised entropy.
            double entropy = entropy_vec[i];

            // We first check if the entropy is less than the minimum.
            // This is important to reduce noise computation (which is not
            // negligible).
            if (entropy <= min) {

                // Then, we add noise to decide randomly which will be chosen.
                // noise is smaller than the smallest p * log(p), so the minimum entropy
                // will always be chosen.
                double noise = dis(gen);
                if (entropy + noise < min) {
                    min = std::min(entropy + noise, min);
                    argmin = i;
                }
            }
        }

        return argmin;
    }

    inline int size() { return _size; };

};

#endif // FAST_WFC_WAVE_HPP_

