#ifndef SRC_DECLARE_HPP
#define SRC_DECLARE_HPP

enum ObserveStatus {
    success, // wfc完成并取得成功
    failure, // wfc完成并失败
    to_continue // wfc没有完成
};


/**
 * A direction is represented by an unsigned integer in the range [0; 3].
 * The x and y values of the direction can be retrieved in these tables.
 */
// TODO 方向的遍历写成迭代器方式
constexpr int directions_x[4] = {0, -1, 1, 0};
constexpr int directions_y[4] = {-1, 0, 0, 1};

/**
 * Return the opposite direction of direction.
 */
constexpr unsigned get_opposite_direction(unsigned direction) noexcept {
    return 3 - direction;
}



struct OverlappingWFCOptions {
    unsigned out_height;  // The height of the output in pixels.
    unsigned out_width;   // The width of the output in pixels.
    unsigned symmetry; // The number of symmetries (the order is defined in wfc).
    unsigned N; // The width and height in pixel of the patterns.
    std::string name; // The width and height in pixel of the patterns.

    unsigned get_wave_height() const noexcept {
        return out_height - N + 1;
    }

    unsigned get_wave_width() const noexcept {
        return out_width - N + 1;
    }
};



#endif