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

    std::string getKey(unsigned index, unsigned pattern) {
        return std::to_string(index) + "_" + std::to_string(pattern);
    }
    template<class T>
    std::string getKey(T id_1, T id_2, T id_3) {
        return std::to_string(id_1) + "_" + std::to_string(id_2) + "_" + std::to_string(id_3);
    }

}
#endif //SRC_UNTI_HPP
