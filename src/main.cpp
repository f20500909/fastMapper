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
#include "Data.hpp"

using namespace std;
/* TODO shell脚本批量生成
 * 数据输入模块，适配多种格式的数据
 *
 */

void single_run(unsigned height, unsigned width, unsigned symmetry, unsigned N, string name) {
    const std::string image_path = "samples/" + name + ".png";
    Data<int> data;

    std::optional<Matrix<Color>> m = data.init(image_path);

    if (!m.has_value()) {
        throw "Error while loading " + image_path;
    }
    OverlappingWFCOptions options = {height, width, symmetry, N};

    srand((unsigned) time(NULL));

    int seed = rand();

    std::vector<Matrix<Color>> patterns;
    std::vector<double> patterns_frequency;
    std::tie(patterns, patterns_frequency) = data.get_patterns(*m, options);

    const std::vector<std::array<std::vector<unsigned>, 4>> propagator = data.generate_compatible(patterns);
    OverlappingWFC<Color> wfc(*m, options, seed, patterns, patterns_frequency, propagator);

    printf("========== run start ==========\n");
    std::optional<Matrix<Color>> success = wfc.run();
    printf("========== run down  ==========\n");
    if (success.has_value()) {
        data.write_image_png("results/" + name + ".png", *success);
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

