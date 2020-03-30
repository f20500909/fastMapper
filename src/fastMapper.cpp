#ifndef SRC_FASTMAPPER_HPP
#define SRC_FASTMAPPER_HPP

#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <ctime>

#include "include/cmdline.h"
#include "wfc.hpp"
#include "imageModel.hpp"
//#include "svg.hpp"

using namespace std;



void single_run(unsigned out_height, unsigned out_width, unsigned symmetry, unsigned N, int channels, string input_data,
                string output_data, string type) {
    srand((unsigned) time(NULL));

//    input_data = "../samples/ai/wh1.svg";
//    type = "svg";

    const Options options = {out_height, out_width, symmetry, N, channels, input_data, output_data, type};

//    Data<int, AbstractFeature> *data = new Svg<int, AbstractFeature>(options);
    Data<int, AbstractFeature> *data = new Img<int, AbstractFeature>(options);

    WFC wfc(data);
    wfc.run();
    delete data;
}

int main(int argc, char *argv[]) {
//    -h 40 -w 40 -s 8  -N 2 -i ./samples/City.png -o ./res/done.jpg  -t img
//    -h 40 -w 40 -s 8  -N 2 -i ../samples/City.png -o ../res/done.jpg  -t svg

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

#endif // SRC_FASTMAPPER_HPP
