#ifndef FAST_WFC_UTILS_ARRAY3D_HPP_
#define FAST_WFC_UTILS_ARRAY3D_HPP_

#include <vector>
#include "declare.hpp"

/**
* Represent a 3D array.
* The 3D array is stored in a single array, to improve cache usage.
*/
template <typename T> class Array3D {

public:
	/**
	* The dimensions of the 3D array.
	*/
	unsigned height;
	unsigned width;
	unsigned depth;

	/**
	* The array containing the data of the 3D array.
	*/
	std::vector<T> data;

	/**
	* Build a 2D array given its height, width and depth.
	* All the arrays elements are initialized to default value.
	*/
	Array3D(unsigned height, unsigned width, unsigned depth) noexcept
		: height(height), width(width), depth(depth),
		data(width* height* depth) {}

	/**
	* Return a const reference to the element in the i-th line, j-th column, and
	* k-th depth. i must be lower than height, j lower than width, and k lower
	* than depth.
	*/
	const T& get(unsigned i, unsigned j, unsigned k) const noexcept {
		assert(i < height && j < width && k < depth);
		return data[i * width * depth + j * depth + k];
	}

	/**
	* Return a reference to the element in the i-th line, j-th column, and k-th
	* depth. i must be lower than height, j lower than width, and k lower than
	* depth.
	*/
	T& get(unsigned i, unsigned j, unsigned k) noexcept {
		return data[i * width * depth + j * depth + k];
	}

	T& get(CoordinateState coor, unsigned k) noexcept {
		int dx = coor.x;
		int dy = coor.y;
		return data[dy * width * depth + dx * depth + k];
	}

};

#endif // FAST_WFC_UTILS_ARRAY3D_HPP_
