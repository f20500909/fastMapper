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

void single_run(unsigned height, unsigned width, unsigned symmetry, unsigned N, string name, int desired_channels) {
    srand((unsigned) time(NULL));
    const std::string image_path = "samples/" + name;
    const int directionSize = 4;

    const Options options = {height, width, symmetry, N, name, image_path, directionSize, desired_channels};

    Data<int>* data = new Img<int>(options);
    WFC wfc(data, options);

    wfc.run();
    delete data;
}

int main(int argc, char *argv[]) {
//    -h 20 -w 100 -s 2  -N 2 -n colored_city
//    -h 40 -w 40 -s 3  -N 2 -n City.png
    cmdline::parser a;
    a.add<unsigned>("height", 'h', "height", true);
    a.add<unsigned>("width", 'w', "width", true);
    a.add<unsigned>("symmetry", 's', "symmetry", true);
    a.add<unsigned>("N", 'N', "N", true);
    a.add<string>("name", 'n', "name", true);
    a.add<int>("channels", 'c', "c", false, 3);
    a.parse_check(argc, argv);

    unsigned height = a.get<unsigned>("height");
    unsigned width = a.get<unsigned>("width");
    unsigned symmetry = a.get<unsigned>("symmetry");
    unsigned N = a.get<unsigned>("N");
    string name = a.get<std::string>("name");
    int channels = a.get<int>("channels");

    cout << "height          : " << a.get<unsigned>("height") << endl
         << "width           : " << a.get<unsigned>("width") << endl
         << "symmetry        : " << a.get<unsigned>("symmetry") << endl
         << "N               : " << a.get<unsigned>("N") << endl
         << "name            : " << a.get<string>("name") << endl
         << "channels            : " << a.get<int>("channels") << endl;

    single_run(height, width, symmetry, N, name, channels);
    return 0;
}


