#ifndef FAST_WFC_OVERLAPPING_WFC_HPP_
#define FAST_WFC_OVERLAPPING_WFC_HPP_

#include <vector>
#include <unordered_map>

#include "Matrix.hpp"
#include "wfc.hpp"
#include "Data.hpp"


/**
* Class generating a new image with the overlapping WFC algorithm.
* 用wfc算法产生一个新的位图
*/
template<typename T>
class Model {

public:
    /**
    * Options needed by the algorithm.
    */
    OverlappingWFCOptions options;


    /**
    * The underlying generic WFC algorithm.
    * 底层的一般wfc算法
    */
    WFC wfc;

    /**
    * Constructor initializing the wfc.
    * This constructor is called by the other constructors.
    * This is necessary in order to initialize wfc only once.
    * 构造函数
    */
    Model(Matrix<Color> &input, const OverlappingWFCOptions &options, std::vector<Matrix<T>> &patterns_1,
          std::vector<double> &patterns_2, const std::vector<std::array<std::vector<unsigned>, 4>> propagator) noexcept
            : options(options),
              wfc(options, patterns_1, patterns_2, propagator, options.get_wave_height(), options.get_wave_width()) {
    }


public:

    /**
    * Run the WFC algorithm, and return the result if the algorithm succeeded.
    * 运行wfc算法，如果成功返回结果
    */
    std::optional<Matrix<T>> run() noexcept {
        std::optional<Matrix<unsigned>> result = wfc.run();
        if (result.has_value()) {
            return wfc.to_image(*result);
        }
        return std::nullopt;
    }
};

#endif // FAST_WFC_WFC_HPP_
