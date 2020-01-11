#ifndef FAST_WFC_UTILS_COLOR_HPP_
#define FAST_WFC_UTILS_COLOR_HPP_

#include <functional>

/**
* Represent a 24-bit rgb color.
*/
class Color {
public:
//	unsigned char r, g, b;
    Color(){
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
    }

    Color(unsigned char r,unsigned char g,unsigned char b) {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }

    unsigned char data[3];

    bool operator==(const Color &c) const noexcept {
        for (int i = 0; i < 3; i++) {
            if (data[i] != c.data[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Color &c) const noexcept { return !(c == *this); }
};

/**
* Hash function for color.
*/
namespace std {
    template<>
    class hash<Color> {
    public:
        size_t operator()(const Color &c) const {
            return (size_t) c.data[0] + (size_t) 256 * (size_t) c.data[1] +
                   (size_t) 256 * (size_t) 256 * (size_t) c.data[2];
        }
    };
} // namespace std

#endif // FAST_WFC_UTILS_COLOR_HPP_
