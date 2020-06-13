#ifndef FAST_WFC_WFC_HPP_
#define FAST_WFC_WFC_HPP_

#include <limits>
#include <unordered_map>
#include <stack>

#include "wave.hpp"
//#include "svg.hpp"

class WFC {
public:
    void run() noexcept {
        init_input_data();
        wave.init_wave();
        while (true) {
            // 定义未定义的网格值  只是观察 返回的是状态
            ObserveStatus result = observe();
            // 检查算法是否结束
            if (result == success) {
                this->show_result(wave_to_output());
                return;
            }

            if (result == failure) {
                this->show_result(wave_to_output());
                std::cout << "failure!!!!!!!!!!!!!!" << std::endl;
                break;
            }
            // 传递信息
            this->propagate();
        }
    }

    Data<int, AbstractFeature> data;

private:
    Wave wave;

    Matrix<unsigned> wave_to_output() noexcept {
        Matrix<unsigned> output_features(conf->wave_height, conf->wave_width);
        for (unsigned i = 0; i < conf->wave_size; i++) {
            for (unsigned k = 0; k < feature.size(); k++) {
                if (wave.get(i, k)) {
                    output_features.get(i) = k;
                }
            }
        }
        return output_features;
    }

    void ban(unsigned wave_id, unsigned fea_id) {
        for (unsigned i = 0; i < _direction.getMaxNumber(); i++) {
            compatible_feature_map[data.getKey(wave_id, fea_id, i)] = 0;
        }
        propagating.push(std::tuple<unsigned int, unsigned int>(wave_id, fea_id));

        wave.ban(wave_id, fea_id, false);
//        std::cout << " wave_min_id " << wave_id << " fea_id " << fea_id << "   " << feature.size() << std::endl;
    }


    ObserveStatus observe() noexcept {
        // 得到具有最低熵的wave_id
        int wave_min_id = success;
        float min = std::numeric_limits<float>::infinity();// float的最小值
        for (unsigned wave_id = 0; wave_id < conf->wave_size; wave_id++) {
            int amount = wave.get_wave_frequency(wave_id);

            float entropy = wave.get_entropy(wave_id);

            if (amount > 1 && entropy < min) {
                min = entropy;
                wave_min_id = wave_id;
            }
        }

        if (wave_min_id == success) {
            return success;
        }

        unsigned sum = wave.get_wave_all_frequency(wave_min_id); //得到此wave 在所有feature中出现的次数的总合
        unsigned chosen_fea_id = wave.get_chosen_value_by_random(wave_min_id, sum);//取wave中的一个fea_id，频率越大，则越有可能被选到

        for (unsigned fea_id = 0; fea_id < feature.size(); fea_id++) {
//            如果wave_min_id对应的图案在argmin中 并且不是选择的元素,就ban了
//            只要不是所选的，都ban了
            if (wave.get(wave_min_id, fea_id) && fea_id != chosen_fea_id) {
                ban(wave_min_id, fea_id);
            }
        }

        //观察结束  继续进行计算
        return to_continue;
    }

    void propagate() noexcept {
        //从最后一个传播状态开始传播,每传播成功一次，就移除一次，直到传播列表为空
        unsigned wave_id, fea_id, wave_next;
        while (!propagating.empty()) {
            // The cell and fea_id that has been set to false.
            std::tie(wave_id, fea_id) = propagating.top();
            propagating.pop();

            //对图案的各个方向进进行传播
            for (unsigned directionId = 0; directionId < _direction.getMaxNumber(); directionId++) {
                //跟具此fea的id 和一个方向id  确定下一个fea的id
                wave_next = _direction.movePatternByDirection(wave_id, directionId, conf->wave_width);

                //只有有效的feature才传播
                if (!this->isVaildPatternId(wave_next)) {
                    continue;
                }

                const auto &temp = propagator[fea_id][directionId];
                for (unsigned fea_id_2 = 0; fea_id_2 < temp.size(); fea_id_2++) {
                    if (!temp.get(fea_id_2)) continue;

                    int &directionCount = data.getDirectionCount(wave_next, fea_id_2, directionId);
                    directionCount--;
                    if (directionCount == 0) {
                        ban(wave_next, fea_id_2);
                    }
                }
            }
        }
    }

    virtual void init_direction() = 0;

    virtual void init_row_data() = 0;

    virtual void init_features() = 0;

    virtual void init_compatible() = 0;

    virtual void init_input_data() {
        init_direction();
        init_row_data();
        init_features();
        init_compatible();
    }


    virtual bool isVaildPatternId(unsigned pId) = 0;

    virtual void show_result(const Matrix<unsigned>& mat) = 0;
};


#endif
