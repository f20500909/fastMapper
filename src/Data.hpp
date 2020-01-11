#ifndef FAST_WFC_UTILS_ARRAY2D_HPP_
#define FAST_WFC_UTILS_ARRAY2D_HPP_

#include "assert.h"
#include <vector>

/**
* Represent a 2D array.
* The 2D array is stored in a single array, to improve cache usage.
*/
template<typename T>
class Data {

public:
    /**
    * Height and width of the 2D array.
    */
    unsigned height;
    unsigned width;

    /**
    * The array containing the data of the 2D array.
    */
    std::vector<T> data;

    /**
    * Build a 2D array given its height and width.
    * All the array elements are initialized to default value.
    */
    Data() {

    };

    Data(unsigned height, unsigned width) noexcept : height(height), width(width), data(width * height) {}

    /**
    * Build a 2D array given its height and width.
    * All the array elements are initialized to value.
    */
    Data(unsigned height, unsigned width, T value) noexcept
            : height(height), width(width), data(width * height, value) {}

    /**
    * Return a const reference to the element in the i-th line and j-th column.
    * i must be lower than height and j lower than width.
    */
    const T &get(unsigned i, unsigned j) const noexcept {
        assert(i < height && j < width);
        return data[j + i * width];
    }

    /**
    * Return a reference to the element in the i-th line and j-th column.
    * i must be lower than height and j lower than width.
    */
    T &get(unsigned i, unsigned j) noexcept {
        assert(i < height && j < width);
        return data[j + i * width];
    }

    /**
    * Return the current 2D array reflected along the x axis.
    */
    Data<T> reflected() const noexcept {
        Data<T> result = Data<T>(width, height);
        for (unsigned y = 0; y < height; y++) {
            for (unsigned x = 0; x < width; x++) {
                result.get(y, x) = get(y, width - 1 - x);
            }
        }
        return result;
    }

    /**
    * Return the current 2D array rotated 90° anticlockwise
    */
    Data<T> rotated() const noexcept {
        Data<T> result = Data<T>(width, height);
        for (unsigned y = 0; y < width; y++) {
            for (unsigned x = 0; x < height; x++) {
                result.get(y, x) = get(x, width - 1 - y);
            }
        }
        return result;
    }

    /**
    * Return the sub 2D array starting from (y,x) and with size (sub_width,
    * sub_height). The current 2D array is considered toric for this operation.
    */
    Data<T> get_sub_array(unsigned y, unsigned x, unsigned sub_width,
                          unsigned sub_height) const noexcept {
        Data<T> sub_array_2d = Data<T>(sub_width, sub_height);
        for (unsigned ki = 0; ki < sub_height; ki++) {
            for (unsigned kj = 0; kj < sub_width; kj++) {
                sub_array_2d.get(ki, kj) = get((y + ki) % height, (x + kj) % width);
            }
        }
        return sub_array_2d;
    }

    /**
    * Assign the matrix a to the current matrix.
    */
    Data<T> &operator=(const Data<T> &a) noexcept {
        height = a.height;
        width = a.width;
        data = a.data;
        return *this;
    }

    /**
    * Check if two 2D arrays are equals.
    */
    bool operator==(const Data<T> &a) const noexcept {
        if (height != a.height || width != a.width) {
            return false;
        }

        for (unsigned i = 0; i < data.size(); i++) {
            if (a.data[i] != data[i]) {
                return false;
            }
        }
        return true;
    }
};

/**
* Hash function.
*/
namespace std {
    template<typename T>
    class hash<Data<T>> {
    public:
        size_t operator()(const Data<T> &a) const noexcept {
            std::size_t seed = a.data.size();
            for (const T &i : a.data) {
                seed ^= hash<T>()(i) + (size_t) 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
} // namespace std

#endif // FAST_WFC_UTILS_ARRAY2D_HPP_
