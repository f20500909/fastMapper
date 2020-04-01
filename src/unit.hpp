#ifndef SRC_UNTI_HPP
#define SRC_UNTI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

namespace unit {

    std::vector<std::string> split_str(std::string str, std::string pattern) {
        std::string::size_type pos;
        std::vector<std::string> result;
        str += pattern;
        size_t size = str.size();
        for (size_t i = 0; i < size; i++) {
            pos = str.find(pattern, i);
            if (pos < size) {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
        return result;
    }


    float get_angle(float x1, float y1, float x2, float y2, float x3, float y3) {
        float theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
        if (theta > M_PI)
            theta -= 2 * M_PI;
        if (theta < -M_PI)
            theta += 2 * M_PI;

        theta = abs(theta * 180.0 / M_PI);
        return theta;
    }

    //获取 p * log(p)
    std::vector<float> get_plogp(const std::vector<float> &distribution) noexcept {
        std::vector<float> plogp(distribution.size(), 0);
        for (unsigned i = 0; i < distribution.size(); i++) {
            plogp[i] = distribution[i] * log(distribution[i]);
        }
        return plogp;
    }


    float get_half_min(const std::vector<float> &v) noexcept {
        float half_min = std::numeric_limits<float>::infinity();
        for (unsigned i = 0; i < v.size(); i++) {
            half_min = std::min(half_min, static_cast<float>(v[i] / 2.0));
        }
        return half_min;
    }

    template<class T>
    double getRand(T min, T max) {

        return min + (max - min) * rand() / static_cast<T>(RAND_MAX);
    }


}
#endif //SRC_UNTI_HPP
