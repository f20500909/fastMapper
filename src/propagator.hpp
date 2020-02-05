#ifndef FAST_WFC_PROPAGATOR_HPP_
#define FAST_WFC_PROPAGATOR_HPP_

#include <tuple>
#include <vector>
#include <array>

#include "array3D.hpp"
#include "wave.hpp"
#include "time.h"
#include "declare.hpp"
#include "direction.hpp"
#include "base.hpp"


/**
 * Propagate information about patterns in the wave.
 */
class Propagator : public Base {
public:
    using PropagatorState = std::vector<std::array<std::vector<unsigned>, directionNumbers>>;

private:
    /**
     * The size of the patterns.
     */
    const unsigned patterns_size;

    /**
     * propagator[pattern1][direction] contains all the patterns that can
     * be placed in next to pattern1 in the direction direction.
     */
    PropagatorState propagator_state;

    /**
     * The wave width and height.
     */
    const unsigned wave_width;
    const unsigned wave_height;
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
    Array3D<std::array<int, directionNumbers>> compatible;

    /**
     * Initialize compatible.
     */
    void init_compatible() noexcept {
        std::array<int, directionNumbers> value;

        auto iterFunc = [&](int direction, int pattern1) {
            value[direction] = propagator_state[pattern1][get_opposite_direction(direction)].size();
        };

        // We compute the number of pattern compatible in all directions.
        for (unsigned y = 0; y < wave_height; y++) {
            for (unsigned x = 0; x < wave_width; x++) {
                for (unsigned pattern = 0; pattern < patterns_size; pattern++) {
                    _direction.doEveryDirectId(std::bind(iterFunc, std::placeholders::_1, std::placeholders::_2),
                                               pattern);
                    compatible.get(y, x, pattern) = value;
                }
            }
        }
    }

public:

    /**
     * Constructor building the propagator and initializing compatible.
     */
    Propagator(unsigned wave_height, unsigned wave_width, PropagatorState propagator_state, const Options &op) noexcept
            : patterns_size(propagator_state.size()), propagator_state(propagator_state), wave_width(wave_width),
              wave_height(wave_height),
              compatible(wave_height, wave_width, patterns_size), Base(op) {
        init_compatible();
    }

    /**
     * Add an element to the propagator.
     * This function is called when wave.get(y, x, pattern) is set to false.
     */
    void add_to_propagator(unsigned y, unsigned x, unsigned pattern) noexcept {
        // All the direction are set to 0, since the pattern cannot be set in (y,x).
        std::array<int, directionNumbers> temp = {};
        compatible.get(y, x, pattern) = temp;
        propagating.emplace_back(y, x, pattern);
    }

    /**
     * Propagate the information given with add_to_propagator.
     * 核心部分，进行传递
     */
    void propagate(Wave &wave) noexcept {

        // We propagate every element while there is element to propagate.
        while (propagating.size() != 0) {

            // The cell and pattern that has been set to false.
            unsigned y1, x1, pattern;
            std::tie(y1, x1, pattern) = propagating.back();
            propagating.pop_back();

//            对图案的四个方向进进行传播
            for (unsigned directionId = 0; directionId < directionNumbers; directionId++) {
                // We get the next cell in the directionId directionId.
                point po = _direction.getPoint(directionId);
                int dx = po[0];
                int dy = po[1];

                int x2 = static_cast<int>(x1) + dx;
                int y2 = static_cast<int>(y1) + dy;

                coordinate coor2 = {x2, y2};

                if (!isVaildCoordinate(coor2)) {
                    continue;
                }

                // The index of the second cell, and the patterns compatible
                unsigned i2 = x2 + y2 * wave.width;
                const std::vector<unsigned> &patterns = propagator_state[pattern][directionId];

                // For every pattern that could be placed in that cell without being in
                // contradiction with pattern1
                for (auto it = patterns.begin(), it_end = patterns.end(); it < it_end; ++it) {

                    // We decrease the number of compatible patterns in the opposite
                    // directionId If the pattern was discarded from the wave, the element
                    // is still negative, which is not a problem
                    std::array<int, directionNumbers> &value = compatible.get(y2, x2, *it);
                    value[directionId]--;

                    // If the element was set to 0 with this operation, we need to remove
                    // the pattern from the wave, and propagate the information
                    if (value[directionId] == 0) {
                        add_to_propagator(y2, x2, *it);
                        wave.set(i2, *it, false);
                    }
                }
            }
        }
    }
};

#endif // FAST_WFC_PROPAGATOR_HPP_
