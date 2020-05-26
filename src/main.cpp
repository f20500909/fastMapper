#include "fastMapper.hpp"
#include "include/cmdline.h"


using namespace std;

int main(int argc, char *argv[]) {
//    -h 40 -w 40 -s 8  -N 2 -i ./samples/City.png -o ./res/done.jpg  -t img
//    -h 40 -w 40 -s 8  -N 2 -i ../samples/City.png -o ../res/done.jpg  -t svg
// -h 20 -w 40 -s 8  -N 2 -i ../samples/o.png -o ../res/done.jpg  -t svg

    cmdline::parser a;
    a.add<unsigned>("height", 'h', "height", true);
    a.add<unsigned>("width", 'w', "width", true);
    a.add<unsigned>("symmetry", 's', "symmetry", true);
    a.add<unsigned>("N", 'N', "N", true);
    a.add<int>("channels", 'c', "c", false, 3);
    a.add<int>("log", 'l', "log", false, 1);
    a.add<string>("input_data", 'i', "input_data", true);
    a.add<string>("output_data", 'o', "output_data", true);
    a.add<string>("type", 't', "type", true);
    a.parse_check(argc, argv);

    unsigned height = a.get<unsigned>("height");
    unsigned width = a.get<unsigned>("width");
    unsigned symmetry = a.get<unsigned>("symmetry");
    unsigned N = a.get<unsigned>("N");
    int channels = a.get<int>("channels");
    int log = a.get<int>("log");
    string input_data = a.get<std::string>("input_data");
    string output_data = a.get<std::string>("output_data");
    string type = a.get<std::string>("type");

    single_run(height, width, symmetry, N, channels, log, input_data, output_data, type);
//    cin.get();
    return 0;
}

