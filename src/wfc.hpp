#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <limits>
#include <unordered_map>
#include <stack>

#include "wave.hpp"
//#include "svg.hpp"

/**
 * Class containing the generic WFC algorithm.
 */
class WFC {
private:
    Data<int, AbstractFeature> *data;

    Wave wave;

    std::stack<std::tuple<unsigned, unsigned>> propagating;
    unordered_map<long long, int> compatible_feature_map;

    /**
     * Transform the wave to a valid output (a 2d array of feature that aren't in contradiction).
     * This function should be used only when all cell of the wave are defined.
     * 将波转换为有效的输出（一个不矛盾的2d阵列）
     * 此函数只有当波的所有格子都被定义
     */
    Matrix<unsigned> wave_to_output() noexcept {
        Matrix<unsigned> output_features(conf->wave_height, conf->wave_width);
        for (unsigned i = 0; i < conf->wave_size; i++) {
            for (unsigned k = 0; k < data->feature.size(); k++) {
                if (wave.get(i, k)) {
                    output_features.get(i) = k;
                }
            }
        }
        return output_features;
    }

    void ban(unsigned wave_id, unsigned fea_id) {
        for (unsigned i = 0; i < data->_direction.getMaxNumber(); i++) {
            compatible_feature_map[data->getKey(wave_id, fea_id, i)] = 0;
        }
        propagating.push({wave_id, fea_id});

        wave.set(wave_id, fea_id, false);

//        std::cout << " wave_min_id " << wave_id << " fea_id " << fea_id << "   " << data->feature.size() << std::endl;
    }


    //没找到 就初始化  那就不用在最初进行初始化了 省了很多事
    int &getDirectionCount(const unsigned &wave_id, const unsigned &fea_id, const unsigned &direction) {
        unordered_map<long long, int>::iterator iter = compatible_feature_map.find(
                data->getKey(wave_id, fea_id, direction));

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
        unsigned wave_id, fea_id, wave_next;
        while (!propagating.empty()) {
            // The cell and fea_id that has been set to false.
            std::tie(wave_id, fea_id) = propagating.top();
            propagating.pop();

            //对图案的各个方向进进行传播
            for (unsigned directionId = 0; directionId < data->_direction.getMaxNumber(); directionId++) {
                //跟具此fea的id 和一个方向id  确定下一个fea的id

                wave_next = data->_direction.movePatternByDirection(wave_id, directionId, conf->wave_width);

                //只有有效的feature才传播
                if (!data->isVaildPatternId(wave_next)) {
                    continue;
                }

                const std::vector<unsigned> &feature = data->propagator[fea_id][directionId];

                for (unsigned i = 0; i < feature.size(); i++) {


                    int &directionCount = getDirectionCount(wave_next, feature[i], directionId);

                    directionCount--;

                    //如果元素被设置为0，就移除此元素,并且将下一方向的元素添加到传播队列
                    //并且将此wave的传播状态设置为不需要传播
                    if (directionCount == 0) {
                        ban(wave_next, feature[i]);
                    }
                }
            }
        }
    }

public:

    WFC(Data<int, AbstractFeature> *data) noexcept : data(data), wave(data) {}

//     运行算法，成功的话并返回一个结果
    void run() noexcept {
        while (true) {
            // 定义未定义的网格值
            ObserveStatus result = observe();
            // 检查算法是否结束
            if (result == success) {
                data->showResult(wave_to_output());
                return;
            }

            if (result == failure) {
                data->showResult(wave_to_output());
                std::cout << "failure!!!!!!!!!!!!!!" << std::endl;
                break;
            }
            // 传递信息
            this->propagate(wave);
        }
    }

    ObserveStatus observe() noexcept {
        // 得到具有最低熵的wave_id
        int wave_min_id = success;
        float min = std::numeric_limits<float>::infinity();// float的最小值

        for (unsigned wave_id = 0; wave_id < conf->wave_size; wave_id++) {

            int amount = wave.features_number_vec[wave_id];

            if (amount == 0){
                data->status = amount_flag;
            }

            float entropy = wave.entropy_vec[wave_id];

            if (amount > 1 && entropy <= min) {
                if(amount_flag==data->status ){
//                    cout << " wave_id " << wave_id << " amount " << amount << endl;
                }

                float noise = unit::getRand(0, 1);  //随机生成一个noise
                if (entropy + noise < min) {
                    min = entropy + noise;  //注意 这里有加了噪点  min值可能没更新
                    wave_min_id = wave_id;
                }
            }
        }

        if (wave_min_id == success) {
            return success;
        }

        unsigned sum = wave.get_wave_all_frequency(wave_min_id); //得到此wave 在所有feature中出现的次数的总合
        unsigned chosen_value = wave.get_chosen_value_by_random(wave_min_id, sum);

//      如果k对应的图案在argmin中 并且不是选择的元素
        for (unsigned fea_id = 0; fea_id < data->feature.size(); fea_id++) {
            if (wave.get(wave_min_id, fea_id) && fea_id != chosen_value) {
                ban(wave_min_id, fea_id);
            }
        }

        //观察结束  继续进行计算
        return to_continue;
    }
};

#endif // FAST_WFC_WFC_HPP_
