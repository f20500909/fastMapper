#ifndef FAST_WFC_OVERLAPPING_WFC_HPP_
#define FAST_WFC_OVERLAPPING_WFC_HPP_

#include <vector>
#include <unordered_map>

#include "Matrix.hpp"
#include "wfc.hpp"
template<typename T>
class Model {

public:


    WFC wfc;

    /**
    * Constructor initializing the wfc.
    * This constructor is called by the other constructors.
    * This is necessary in order to initialize wfc only once.
    * 构造函数
    */
    Model(Data<int> data, const OverlappingWFCOptions &options)
            : wfc(options, data.patterns, data.patterns_frequency, data.propagator, options.get_wave_height(), options.get_wave_width()) {
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
