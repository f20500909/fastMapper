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

}
#endif //SRC_UNTI_HPP
