#ifndef FAST_WFC_PROPAGATOR_HPP_
#define FAST_WFC_PROPAGATOR_HPP_

#include <tuple>
#include <vector>
#include <array>
#include <functional>

#include "wave.hpp"
#include "svg.hpp"

/**
 * Propagate information about feature in the wave.
 */
class Propagator {
private:

    std::vector<std::tuple<unsigned, unsigned>> propagating;

    Array2D<std::vector<int>> compatible;

    void init_compatible() noexcept {

        //可能的图案id

        //储存的信息 -> 每一个输出元素中 的 每一个特征的数量
        compatible = Array2D<std::vector<int>>(data->options.wave_size, data->feature.size());

        //对所有输出的尺寸
        for (unsigned id = 0; id < data->options.wave_size; id++) {
            //对所有提取的特征图案id
            for (unsigned pattern = 0; pattern < data->feature.size(); pattern++) {

                std::vector<int> value(data->_direction.getMaxNumber());

                //对特征图案的所有方向
                for (int direction = 0; direction < data->_direction.getMaxNumber(); direction++) {
                    //对特征图案的所有方向的相反方向
                    unsigned oppositeDirection = data->_direction.get_opposite_direction(direction);
                    //此方向上的值  等于 其反方向上的可传播大小
                    value[direction] = data->propagator[pattern][oppositeDirection].size();
                }

                //设置一个 id 对应图案id 的 特征频次
                compatible.get(id, pattern) = value;
            }
        }
    }

    Data<int, AbstractFeature> *data;

public:

    Propagator(Data<int, AbstractFeature> *data) noexcept : data(data) {
        init_compatible();
    }

    void add_to_propagator(unsigned fea_id_1, unsigned fea_id_2) noexcept {
        // All the direction are set to 0, since the pattern cannot be set in (y,x).
        compatible.get(fea_id_1, fea_id_2) = std::vector<int>(data->_direction.getMaxNumber(), 0);
        propagating.emplace_back(fea_id_1, fea_id_2);
    }

    // 核心部分，进行传递
    void propagate(Wave &wave) noexcept {
        //从最后一个传播状态开始传播,每传播成功一次，就移除一次，直到传播列表为空
        while (!propagating.empty()) {
            // The cell and pattern that has been set to false.
            unsigned fea_id_1, fea_id_2, fea_id_3;
            std::tie(fea_id_1, fea_id_2) = propagating.back();
            propagating.pop_back();

            //对图案的各个方向进进行传播
            for (unsigned directionId = 0; directionId < data->_direction.getMaxNumber(); directionId++) {
                //更具此fea的id 和一个方向id  确定下一个fea的id

                auto temp = this->data->feature[fea_id_1];
                fea_id_3 = temp.direction_fea_id_vec[directionId];

                //只有有效的feature才传播
                if (!data->isVaildPatternId(fea_id_3)) {
                    continue;
                }

                // The index of the second cell, and the feature compatible
                const std::vector<unsigned> &feature = data->propagator[fea_id_2][directionId];

                // For every pattern that could be placed in that cell without being in
                // contradiction with feature1
                for (unsigned i = 0; i < feature.size(); i++) {
                    // We decrease the number of compatible feature in the opposite
                    // directionId If the pattern was discarded from the wave, the element
                    // is still negative, which is not a problem

                    std::vector<int> &value = compatible.get(fea_id_3, feature[i]);

                    //方向自减
                    value[directionId]--;

                    //如果元素被设置为0，就移除此元素,并且将下一方向的元素添加到传播队列
                    //并且将此wave的传播状态设置为不需要传播
                    if (value[directionId] == 0) {
                        add_to_propagator(fea_id_3, feature[i]);
                        wave.set(fea_id_3, feature[i], false);
                    }
                }
            }
        }
    }
};

#endif // FAST_WFC_PROPAGATOR_HPP_
