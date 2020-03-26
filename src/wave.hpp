#ifndef FAST_WFC_WAVE_HPP_
#define FAST_WFC_WAVE_HPP_

#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "data.hpp"

class Wave  {
private:


    /**
    * The precomputation of p * log(p).
    * p*log（p）的值
    */
    const std::vector<double> plogp_features_frequency;

    /**
    * The precomputation of min (p * log(p)) / 2.
    * This is used to define the maximum value of the noise.
    */
    const double half_min_plogp;

    /**
    * This value is set to true if there is a contradiction in the wave (all elements set to false in a cell).
    * 如果wave存在矛盾（在单元格中所有元素都设置为false），则这个值设置为true
    */
    bool is_impossible;

    /**
    * The number of distinct feature.
    * 不同形状的图案数量
    */
    const unsigned feature_number;

    /**
    * The actual wave. mat.get(index, pattern) is equal to 0 if the pattern can
    * be placed in the cell index.
    */
    Matrix<uint8_t> mat;

    const Data<int,AbstractFeature> *data;

    /**
    * Return distribution * log(distribution).
    * 计算p*log（p）
    */
    static std::vector<double>
    get_plogp(const std::vector<double> &distribution) noexcept {
        std::vector<double> plogp;
        for (unsigned i = 0; i < distribution.size(); i++) {
            plogp.push_back(distribution[i] * log(distribution[i]));
        }
        return plogp;
    }

    /**
    * Return min(v) / 2.
    * 返回最小值 / 2
    */
    static double get_half_min(const std::vector<double> &v) noexcept {
        double half_min = std::numeric_limits<double>::infinity();
        for (unsigned i = 0; i < v.size(); i++) {
            half_min = std::min(half_min, v[i] / 2.0);
        }
        return half_min;
    }


    std::vector<double> p_log_p_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;       // The sum of p'(pattern).
    std::vector<double> log_sum;   // The log of sum.
    std::vector<unsigned> features_number_vec; // The number of feature present
    std::vector<double> entropy_vec;       // The entropy of the cell.c

public:

    const unsigned size;

    /**
    * Initialize the wave with every cell being able to have every pattern.
    * 初始化wave中每个cell
    */
    Wave( Data<int,AbstractFeature> *data) noexcept
            : plogp_features_frequency(get_plogp(data->features_frequency)),
              half_min_plogp(get_half_min(plogp_features_frequency)), is_impossible(false),
              feature_number(data->features_frequency.size()), mat(data->options.wave_size, feature_number, 1),
              size(data->options.wave_size), data(data) {

        double base_entropy = 0;
        double base_s = 0;

        for (unsigned i = 0; i < feature_number; i++) {
            base_entropy += plogp_features_frequency[i];// plogp 的和
            base_s += data->features_frequency[i];// 频率的和
        }
        double log_base_s = log(base_s);
        double entropy_base = log_base_s - base_entropy / base_s;

        p_log_p_sum = std::vector<double>(size, base_entropy);
        sum = std::vector<double>(size, base_s);
        log_sum = std::vector<double>(size, log_base_s);
        features_number_vec = std::vector<unsigned>(size, feature_number);
        entropy_vec = std::vector<double>(size, entropy_base);
    }

    /**
    * Return true if pattern can be placed in cell index.
    * 返回true如果图案能放入cell
    */
    bool get(unsigned index, unsigned pattern) const noexcept {
        return mat.get(index, pattern);
    }



    const double get_features_frequency(unsigned feature_id, unsigned i) const {
        return this->get(feature_id, i) ? data->features_frequency[i] : 0;
    }

    /**
    * Return true if pattern can be placed in cell (i,j)
    * 返回true如果图案能放进cell（i，j）
    */
    bool get(unsigned i, unsigned j, unsigned pattern) const noexcept {
        return get(i *data-> options.wave_width + j, pattern);
    }

    /**
    * Set the value of pattern in cell index.
    * 设置图案在cell索引中的值
    */
    void set(unsigned index, unsigned pattern, bool value) noexcept {
        bool old_value = mat.get(index, pattern);
        // If the value isn't changed, nothing needs to be done.
        if (old_value == value) return;

        // Otherwise, the memoisation should be updated.
        mat.get(index, pattern) = value;
        p_log_p_sum[index] -= plogp_features_frequency[pattern];
        sum[index] -= data->features_frequency[pattern];
        log_sum[index] = log(sum[index]);
        features_number_vec[index]--;
        entropy_vec[index] = log_sum[index] - p_log_p_sum[index] / sum[index];
        // If there is no feature possible in the cell, then there is a
        // contradiction.
        if (features_number_vec[index] == 0) is_impossible = true;
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
        int argmin =success;

        for (int i = 0; i < size; i++) {
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

};

#endif // FAST_WFC_WAVE_HPP_

