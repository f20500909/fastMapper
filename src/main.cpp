#include <iostream>
#include <random>
#include <string>
#include <unistd.h>
#include <unordered_set>

#include "include/cmdline.h"
#include "image.hpp"

#include "Matrix.hpp"
#include "wfc.hpp"

using namespace std;

/* TODO shell脚本批量生成
 * 数据输入模块，适配多种格式的数据
 */

void single_run(unsigned height, unsigned width, unsigned symmetry, unsigned N, string name) {
    srand((unsigned) time(NULL));
    const std::string image_path = "samples/" + name + ".png";

    const OverlappingWFCOptions options = {height, width, symmetry, N};
    Data<int> data(options);

    data.init(image_path);

    WFC wfc(data, options);

    Matrix<Cell> success = wfc.run();

//    Matrix<Cell> success =data.to_image(res);
    if (success.data.size() > 0) {
        data.write_image_png("results/" + name + ".png", success);
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

