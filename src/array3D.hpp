#ifndef FAST_WFC_UTILS_ARRAY3D_HPP_
#define FAST_WFC_UTILS_ARRAY3D_HPP_

#include <vector>
#include "declare.hpp"


template<typename T>
class Array2D {
public:
    unsigned height;
    unsigned width;
    unsigned depth;


    std::vector<T> data;

    Array2D() {

    }

    /**
    * Build a 2D array given its height, width and depth.
    * All the arrays elements are initialized to default value.
    */
    Array2D(unsigned height, unsigned width, unsigned depth) noexcept
            : height(height), width(width), depth(depth),
              data(width * height * depth) {}


    /**
    * Return a reference to the element in the i-th line, j-th column, and k-th
    * depth. i must be lower than height, j lower than width, and k lower than
    * depth.
    */
    T &get(unsigned i, unsigned j, unsigned k) noexcept {
        return data[i * width * depth + j * depth + k];
    }


    T &get(unsigned id, unsigned k) noexcept {
        return data[depth * (id) + k];
    }
};


#endif // FAST_WFC_UTILS_ARRAY3D_HPP_
