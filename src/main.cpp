#include <iostream>
#include <random>
#include <string>
//#include <conio.h>
#include <unistd.h>
#include <unordered_set>

#include "time.h"
#include "include/cmdline.h"
#include "overlapping_wfc.hpp"
#include "image.hpp"

using namespace std;
//TODO shell脚本批量生成

/**
* Return true if the pattern1 is compatible with pattern2
* when pattern2 is at a distance (dy,dx) from pattern1.
* 当两个图案距离dy，dx时检测是否匹配，在此距离下是否相等
*/
static bool isEpual(const Data<Color> &pattern1, const Data<Color> &pattern2,
                    int dy, int dx) noexcept {
    unsigned xmin = max(0, dx);
    unsigned xmax = dx < 0 ? dx + pattern2.width : pattern1.width;
    unsigned ymin = max(0, dy);
    unsigned ymax = dy < 0 ? dy + pattern2.height : pattern1.width;

    // Iterate on every pixel contained in the intersection of the two pattern.
    // 迭代两个图案中每个像素
    for (unsigned y = ymin; y < ymax; y++) {
        for (unsigned x = xmin; x < xmax; x++) {
            // Check if the color is the same in the two patterns in that pixel.
            // 检查是否颜色相同
            if (pattern1.get(y, x) != pattern2.get(y - dy, x - dx)) {
                return false;
            }
        }
    }
    return true;
}

/**
* Precompute the function isEpual(pattern1, pattern2, dy, dx).
* If isEpual(pattern1, pattern2, dy, dx), then compatible[pattern1][direction]
* contains pattern2, where direction is the direction defined by (dy, dx) (see direction.hpp).
* 先计算是否匹配
 如果匹配，则合并
*/
static std::vector<std::array<std::vector<unsigned>, 4>>
generate_compatible(const std::vector<Data<Color>> &patterns) noexcept {
    std::vector<std::array<std::vector<unsigned>, 4>> compatible = std::vector<std::array<std::vector<unsigned>, 4>>(
            patterns.size());
    // Iterate on every dy, dx, pattern1 and pattern2
    // 对每个图案
    for (unsigned pattern1 = 0; pattern1 < patterns.size(); pattern1++) {
        // 对上下左右四个方向
        for (unsigned direction = 0; direction < 4; direction++) {
            // 对所需要比较的每个图案
            for (unsigned pattern2 = 0; pattern2 < patterns.size(); pattern2++) {
                //判断是否相等
                if (isEpual(patterns[pattern1], patterns[pattern2], directions_y[direction], directions_x[direction])) {
                    //判断是否相等，如果相等则赋值记录
                    compatible[pattern1][direction].push_back(pattern2);
                }
            }
        }
    }
    return compatible;
}

/**
* Return the list of patterns, as well as their probabilities of apparition.
* 返回图案列表，以及它出现的概率
*/
static std::tuple<std::vector<Data<Color>>, std::vector<double>>
get_patterns(const Data<Color> &input, const OverlappingWFCOptions &options) noexcept {
    std::unordered_map<Data<Color>, unsigned> patterns_id;
    std::vector<Data<Color>> patterns;

    // The number of time a pattern is seen in the input image.
    // 一个图案在输入中出现的次数
    std::vector<double> patterns_frequency;

    std::vector<Data<Color>> symmetries(8, Data<Color>(options.N, options.N));
    unsigned max_i = input.height - options.N + 1;
    unsigned max_j = input.width - options.N + 1;

    for (unsigned i = 0; i < max_i; i++) {
        for (unsigned j = 0; j < max_j; j++) {
            // Compute the symmetries of every pattern in the image.
            // 计算此图案的其他形式，旋转，对称
            symmetries[0].data = input.get_sub_array(i, j, options.N, options.N).data;
            symmetries[1].data = symmetries[0].reflected().data;
            symmetries[2].data = symmetries[0].rotated().data;
            symmetries[3].data = symmetries[2].reflected().data;
            symmetries[4].data = symmetries[2].rotated().data;
            symmetries[5].data = symmetries[4].reflected().data;
            symmetries[6].data = symmetries[4].rotated().data;
            symmetries[7].data = symmetries[6].reflected().data;

            // The number of symmetries in the option class define which symetries will be used.
            // 哪些对称将被使用
            for (unsigned k = 0; k < options.symmetry; k++) {
                auto res = patterns_id.insert(
                        std::make_pair(symmetries[k], patterns.size()));

                // If the pattern already exist, we just have to increase its number of appearance.
                // 如果图案已经存在，我们只需提高他的出现率
                if (!res.second) {
                    patterns_frequency[res.first->second] += 1;
                } else {
                    patterns.push_back(symmetries[k]);
                    patterns_frequency.push_back(1);
                }
            }
        }
    }
    return {patterns, patterns_frequency};
}

void single_run(unsigned height, unsigned width, unsigned symmetry, unsigned N, string name) {
    const std::string image_path = "samples/" + name + ".png";
    std::optional<Data<Color>> m = read_image(image_path);
    if (!m.has_value()) {
        throw "Error while loading " + image_path;
    }
    OverlappingWFCOptions options = {height, width, symmetry, N};

    srand((unsigned) time(NULL));

    int seed = rand();

    std::vector<Data<Color>> patterns;
    std::vector<double> patterns_frequency;
    std::tie(patterns, patterns_frequency) = get_patterns(*m, options);

    const std::vector<std::array<std::vector<unsigned>, 4>> propagator = generate_compatible(patterns);
    OverlappingWFC<Color> wfc(*m, options, seed, patterns, patterns_frequency, propagator);

    printf("========== run start ==========\n");
    std::optional<Data<Color>> success = wfc.run();
    printf("========== run down  ==========\n");
    if (success.has_value()) {
        write_image_png("results/" + name + ".png", *success);
        cout << name << " finished!" << endl;
    } else {
        cout << "failed!" << endl;
    }
}

int main(int argc, char *argv[]) {
//    -h 20 -w 100 -s 2  -N 2 -n colored_city

    cmdline::parser a;
    a.add<unsigned>("height", 'h', "height", true);
    a.add<unsigned>("width", 'w', "width", true);
    a.add<unsigned>("symmetry", 's', "symmetry", true);
    a.add<unsigned>("N", 'N', "N", true);
    a.add<string>("name", 'n', "name", true);
    a.parse_check(argc, argv);


    unsigned height = a.get<unsigned>("height");
    unsigned width = a.get<unsigned>("width");
    unsigned symmetry = a.get<unsigned>("symmetry");
    unsigned N = a.get<unsigned>("N");
    string name = a.get<std::string>("name");


    cout << "height          : " << a.get<unsigned>("height") << endl
         << "width           : " << a.get<unsigned>("width") << endl
         << "symmetry        : " << a.get<unsigned>("symmetry") << endl
         << "N               : " << a.get<unsigned>("N") << endl
         << "name            : " << a.get<string>("name") << endl;

    single_run(height, width, symmetry, N, name);
    return 0;
}

