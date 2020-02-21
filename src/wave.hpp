#ifndef FAST_WFC_WAVE_HPP_
#define FAST_WFC_WAVE_HPP_

#include <iostream>
#include <limits>
#include <math.h>
#include <random>
#include <stdint.h>
#include <vector>

/**
* Contains the pattern possibilities in every cell.
* Also contains information about cell entropy.
* 包含每个格子的图案可能性
* 包含网格信息熵
*/

class Wave : public Base {
private:


    /**
    * The precomputation of p * log(p).
    * p*log（p）的值
    */
    const std::vector<double> plogp_patterns_frequency;

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
    * The number of distinct patterns.
    * 不同形状的图案数量
    */
    const unsigned nb_patterns;

    /**
    * The actual wave. mat.get(index, pattern) is equal to 0 if the pattern can
    * be placed in the cell index.
    */
    Matrix<uint8_t> mat;

    Data<int> *data;

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

public:
    /**
    * The size of the wave.
    * wave的尺寸
    */
    const unsigned size;

    /**
    * Initialize the wave with every cell being able to have every pattern.
    * 初始化wave中每个cell
    */
    Wave(Options op, Data<int> *data) noexcept
            : plogp_patterns_frequency(get_plogp(data->patterns_frequency)),
              half_min_plogp(get_half_min(plogp_patterns_frequency)), is_impossible(false),
              nb_patterns(data->patterns_frequency.size()), mat(op.wave_width * op.wave_height, nb_patterns, 1),
              size(options.wave_height * options.wave_width), Base(op), data(data) {

        double base_entropy = 0;
        double base_s = 0;
        double half_min_plogp = std::numeric_limits<double>::infinity();

        for (unsigned i = 0; i < nb_patterns; i++) {
            half_min_plogp = std::min(half_min_plogp, plogp_patterns_frequency[i] / 2.0);
            base_entropy += plogp_patterns_frequency[i];// plogp 的和
            base_s += data->patterns_frequency[i];// 频率的和
        }
        double log_base_s = log(base_s);
        double entropy_base = log_base_s - base_entropy / base_s;

        plogp_sum = std::vector<double>(size, base_entropy);
        sum = std::vector<double>(size, base_s);
        log_sum = std::vector<double>(size, log_base_s);
        nb_patterns_vec = std::vector<unsigned>(size, nb_patterns);
        entropy_vec = std::vector<double>(size, entropy_base);
    }

    /**
    * Return true if pattern can be placed in cell index.
    * 返回true如果图案能放入cell
    */
    bool get(unsigned index, unsigned pattern) const noexcept {
        return mat.get(index, pattern);
    }

    /**
    * Return true if pattern can be placed in cell (i,j)
    * 返回true如果图案能放进cell（i，j）
    */
    bool get(unsigned i, unsigned j, unsigned pattern) const noexcept {
        return get(i * options.wave_width + j, pattern);
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
        plogp_sum[index] -= plogp_patterns_frequency[pattern];
        sum[index] -= data->patterns_frequency[pattern];
        log_sum[index] = log(sum[index]);
        nb_patterns_vec[index]--;
        entropy_vec[index] = log_sum[index] - plogp_sum[index] / sum[index];
        // If there is no patterns possible in the cell, then there is a
        // contradiction.
        if (nb_patterns_vec[index] == 0) is_impossible = true;
    }

    /**
    * Set the value of pattern in cell index.
    * 设置图案在cell索引中的值
    */
    void set(CoordinateState coor, unsigned pattern, bool value) noexcept {
        unsigned index = coor.x + coor.y * this->options.wave_width;
        set(index, pattern, value);
    }

    /**
    * Set the value of pattern in cell (i,j).
    * 设置图案在cell（i，j）的值
    */
    void set(unsigned i, unsigned j, unsigned pattern, bool value) noexcept {
        set(i * options.wave_width + j, pattern, value);
    }

    /**
    * 返回不为0的最小熵的索引
    * 如果中间有contradiction在wave中，则返回-2
    * 如果所有cell都被定义，返回-1
    */
    int get_min_entropy(std::minstd_rand &gen) const noexcept {
        if (is_impossible) {
            return -2;
        }

        std::uniform_real_distribution<> dis(0, abs(half_min_plogp));

        double min = std::numeric_limits<double>::infinity();
        int argmin = -1;

        for (int i = 0; i < size; i++) {
            // If the cell is decided, we do not compute the entropy (which is equal to 0).
            // 如果cell被决定，我们不用再计算信息熵
            double nb_patterns = nb_patterns_vec[i];
            if (nb_patterns == 1) {
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


    std::vector<double> plogp_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;       // The sum of p'(pattern).
    std::vector<double> log_sum;   // The log of sum.
    std::vector<unsigned> nb_patterns_vec; // The number of patterns present
    std::vector<double> entropy_vec;       // The entropy of the cell.c
};

#endif // FAST_WFC_WAVE_HPP_

