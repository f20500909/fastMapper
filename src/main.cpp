#include <iostream>
#include <random>
#include <string>
#include <unistd.h>
#include <unordered_set>

#include "include/cmdline.h"

#include "wfc.hpp"
#include "svg.hpp"

using namespace std;

/* TODO shell脚本批量生成
 * 数据输入模块，适配多种格式的数据
 */

void single_run(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels, string input_data,
                string output_data, string type) {
    srand((unsigned) time(NULL));

    input_data = "./samples/ai/wh1.svg";
    type = "svg";
    const Options options = {out_height, out_width, symmetry, N, channels, input_data, output_data, type};

    Data<int> *data;
    if (options.type == "svg") {
        std::cout<<"use svg modle "<<std::endl;
        data = new Svg<int>(options);
    } else if (options.type=="img")  {
        data = new Img<int>(options);
    }else{
        assert(!"type err...");
    }
    WFC wfc(data, options);
    wfc.run();

    delete data;
}

int main(int argc, char *argv[]) {
//    -h 40 -w 40 -s 8  -N 2 -n City.png
    cmdline::parser a;
    a.add<unsigned>("height", 'h', "height", true);
    a.add<unsigned>("width", 'w', "width", true);
    a.add<unsigned>("symmetry", 's', "symmetry", true);
    a.add<unsigned>("N", 'N', "N", true);
    a.add<int>("channels", 'c', "c", false, 3);
    a.add<string>("input_data", 'i', "input_data", true);
    a.add<string>("output_data", 'o', "output_data", true);
    a.add<string>("type", 't', "type", true);
    a.parse_check(argc, argv);

    unsigned height = a.get<unsigned>("height");
    unsigned width = a.get<unsigned>("width");
    unsigned symmetry = a.get<unsigned>("symmetry");
    unsigned N = a.get<unsigned>("N");
    int channels = a.get<int>("channels");
    string input_data = a.get<std::string>("input_data");
    string output_data = a.get<std::string>("output_data");
    string type = a.get<std::string>("type");

    cout << "height                   : " << a.get<unsigned>("height") << endl
         << "width                    : " << a.get<unsigned>("width") << endl
         << "symmetry                 : " << a.get<unsigned>("symmetry") << endl
         << "N                        : " << a.get<unsigned>("N") << endl
         << "channels                 : " << a.get<int>("channels") << endl
         << "input_data               : " << a.get<string>("input_data") << endl
         << "output_data              : " << a.get<string>("output_data") << endl
         << "type                     : " << a.get<string>("type") << endl;

    single_run(height, width, symmetry, N, channels, input_data, output_data, type);
    return 0;
}


