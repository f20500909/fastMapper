#ifndef FAST_WFC_PROPAGATOR_HPP_
#define FAST_WFC_PROPAGATOR_HPP_

#include <tuple>
#include <vector>
#include <array>
#include <functional>

#include "array3D.hpp"
#include "wave.hpp"

/**
 * Propagate information about patterns in the wave.
 */
class Propagator {
public:
private:
    /**
     * propagator[pattern1][direction] contains all the patterns that can
     * be placed in next to pattern1 in the direction direction.
     */

    /**
     * The wave width and height.
     */
    /**
     * All the tuples (y, x, pattern) that should be propagated.
     * The tuple should be propagated when wave.get(y, x, pattern) is set to
     * false.
     */
    std::vector<std::tuple<unsigned, unsigned, unsigned>> propagating;

    /**
     * compatible.get(y, x, pattern)[direction] contains the number of patterns
     * present in the wave that can be placed in the cell next to (y,x) in the
     * opposite direction of direction without being in contradiction with pattern
     * placed in (y,x). If wave.get(y, x, pattern) is set to false, then
     * compatible.get(y, x, pattern) has every element negative or null
     */
    Array3D<std::vector<int>> compatible;

    /**
     * Initialize compatible.
     */
    void init_compatible() noexcept {
        std::vector<int> value( data->_direction.getMaxNumber());

        compatible = Array3D<std::vector<int>>(data->options.wave_height, data->options.wave_width, data->patterns.size());

        for (unsigned id = 0; id < data->options.wave_size; id++) {
            for (unsigned pattern = 0; pattern < data->patterns.size(); pattern++) {
                for (int direction = 0; direction <  data->_direction.getMaxNumber(); direction++) {
                    unsigned oppositeDirection = data->_direction.get_opposite_direction(direction);
                    value[direction] = data->propagator[pattern][oppositeDirection].size();
                }

                unsigned x = id % data->options.wave_width;
                unsigned y = id / data->options.wave_width;

                CoordinateState coor(x, y);
                assert(x < data->options.wave_width);
                assert(y < data->options.wave_height);

                compatible.get(coor, pattern) = value;

            }
        }
    }

    Data<int, AbstractFeature> *data;

public:

    /**
     * Constructor building the propagator and initializing compatible.
     */
    Propagator(Data<int, AbstractFeature> *data) noexcept : data(data) {
        init_compatible();
    }

    void add_to_propagator(CoordinateState coor, unsigned pattern) noexcept {
        // All the direction are set to 0, since the pattern cannot be set in (y,x).
        compatible.get(coor, pattern) = std::vector<int>( data->_direction.getMaxNumber(),0);
        propagating.emplace_back(coor.y, coor.x, pattern);
    }

    // 核心部分，进行传递
    void propagate(Wave &wave) noexcept {
        //从最后一个传播状态开始传播,没传播成功一次，就移除一次，直到传播列表为空
        while (!propagating.empty()) {
            // The cell and pattern that has been set to false.
            unsigned y1, x1, pattern;
            std::tie(y1, x1, pattern) = propagating.back();
            propagating.pop_back();

            CoordinateState coor1(x1, y1);

            //对图案的各个方向进进行传播
            for (unsigned directionId = 0; directionId <  data->_direction.getMaxNumber(); directionId++) {
                Direction po = data->_direction.getDirectionFromId(directionId);

                CoordinateState coor2 = coor1.getNextDirection(po);

                if (!data->isVaildCoordinate(coor2)) {
                    continue;
                }

                // The index of the second cell, and the patterns compatible
                const std::vector<unsigned> &patterns = data->propagator[pattern][directionId];

                // For every pattern that could be placed in that cell without being in
                // contradiction with pattern1
                for (auto it = patterns.begin(), it_end = patterns.end(); it < it_end; ++it) {
                    // We decrease the number of compatible patterns in the opposite
                    // directionId If the pattern was discarded from the wave, the element
                    // is still negative, which is not a problem
                    std::vector<int> &value = compatible.get(coor2, *it);
                    //方向自减
                    value[directionId]--;

                    //如果元素被设置为0，就移除此元素,并且将下一方向的元素添加到传播队列
                    //并且将此wave的传播状态设置为不需要传播
                    if (value[directionId] == 0) {
                        add_to_propagator(coor2, *it);
                        wave.set(coor2, *it, false);
                    }
                }
            }
        }
    }
};

#endif // FAST_WFC_PROPAGATOR_HPP_
