#ifndef FAST_WFC_PROPAGATOR_HPP_
#define FAST_WFC_PROPAGATOR_HPP_

#include <tuple>
#include <vector>
#include <array>
#include <functional>

#include "wave.hpp"
#include "data.hpp"
//#include "svg.hpp"

class Propagator {
private:

    std::vector<std::tuple<unsigned, unsigned>> propagating;
    unordered_map<long long, int> compatible_feature_map;


    Data<int, AbstractFeature> *data;

public:

    Propagator(Data<int, AbstractFeature> *data) noexcept : data(data) {
    }


    void add_to_propagator(unsigned fea_id_1, unsigned fea_id_2) noexcept {
        for (unsigned i = 0; i < data->_direction.getMaxNumber(); i++) {
            compatible_feature_map[data->getKey(fea_id_1, fea_id_2, i)] = 0;
        }
        propagating.emplace_back(fea_id_1, fea_id_2);
    }

    //没找到 就初始化  那就不用在最初进行初始化了 省了很多事
    int &getDirectionCount(const unsigned &wave_id, const unsigned &fea_id, const unsigned &direction) {
        unordered_map<long long, int>::iterator iter = compatible_feature_map.find(data->getKey(wave_id, fea_id, direction));

        if (iter == compatible_feature_map.end()) {
            unsigned oppositeDirection = data->_direction.get_opposite_direction(direction);
            //此方向上的值  等于 其反方向上的可传播大小
            long long key = data->getKey(wave_id, fea_id, direction);

            compatible_feature_map[key] = data->propagator[fea_id][oppositeDirection].size();
            return compatible_feature_map[key];
        }

        return iter->second;
    }

    void propagate(Wave &wave) noexcept {
        //从最后一个传播状态开始传播,每传播成功一次，就移除一次，直到传播列表为空
        unsigned fea_id_1, fea_id_2, fea_id_3;
        while (!propagating.empty()) {
            // The cell and fea_id that has been set to false.
            std::tie(fea_id_1, fea_id_2) = propagating.back();
            propagating.pop_back();

            //对图案的各个方向进进行传播
            for (unsigned directionId = 0; directionId < data->_direction.getMaxNumber(); directionId++) {
                //跟具此fea的id 和一个方向id  确定下一个fea的id
//                fea_id_3 = fea_id_1 + data->_direction.movePatternByDirection(directionId, data->options.wave_width);

                fea_id_3 = data->_direction.movePatternByDirection(fea_id_1, directionId, data->options.wave_width);

                //只有有效的feature才传播
                if (!data->isVaildPatternId(fea_id_3)) {
                    continue;
                }

                // The index of the second cell, and the feature compatible_feature
                const std::vector<unsigned> &feature = data->propagator[fea_id_2][directionId];

                // For every fea_id that could be placed in that cell without being in
                // contradiction with feature1
                for (unsigned i = 0; i < feature.size(); i++) {
                    // We decrease the number of compatible_feature feature in the opposite
                    // directionId If the fea_id was discarded from the wave, the element
                    // is still negative, which is not a problem

                    int &directionCount = getDirectionCount(fea_id_3, feature[i], directionId);

                    //方向自减
                    directionCount--;

                    //如果元素被设置为0，就移除此元素,并且将下一方向的元素添加到传播队列
                    //并且将此wave的传播状态设置为不需要传播
                    if (directionCount == 0) {
                        add_to_propagator(fea_id_3, feature[i]);
                        wave.set(fea_id_3, feature[i], false);
                    }
                }
            }
        }
    }
};

#endif // FAST_WFC_PROPAGATOR_HPP_
