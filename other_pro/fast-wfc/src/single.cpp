#include <iostream>
#include <random>
#include <string>
#include <conio.h>
#include <unistd.h>
#include <unordered_set>

#include "time.h"
#include "cmdline.h"
#include "overlapping_wfc.hpp"
#include "image.hpp"

using namespace std;

void
single_run(bool periodic_input, bool periodic_output, unsigned height, unsigned width, unsigned symmetry, bool ground,
           unsigned N, string name) {
    const std::string image_path = "samples/" + name + ".png";
    std::optional<Array2D<Color>> m = read_image(image_path);
    if (!m.has_value()) {
        throw "Error while loading " + image_path;
    }
    OverlappingWFCOptions options = {periodic_input, periodic_output, height, width, symmetry, ground, N};

    srand((unsigned) time(NULL));

    int seed = rand();

    printf("========== init start ==========\n");
    OverlappingWFC<Color> wfc(*m, options, seed);
    printf("========== init down ==========\n");

    printf("========== run start ==========\n");
    std::optional<Array2D<Color>> success = wfc.run();
    printf("========== run down  ==========\n");
    if (success.has_value()) {
        write_image_png("results/" + name + ".png", *success);
        cout << name << " finished!" << endl;
    } else {
        cout << "failed!" << endl;
    }
}


int main(int argc, char *argv[]) {
//    1 1 48 48 8 0 2
//    1 1 48 48 2 1 3
//    periodic_input, periodic_output, height, width, symmetry, ground, N
//    -i 1 -o 1 -h 48 -w 48 -s 8 -g 0 -N 2 -n 3Bricks
//    -i 1 -o 1 -h 20 -w 100 -s 1 -g 0 -N 3 -n 3Bricks

    cmdline::parser a;
    a.add<bool>("periodic_input", 'i', "periodic_input", true);
    a.add<bool>("periodic_output", 'o', "periodic_output", true);
    a.add<unsigned>("height", 'h', "height", true);
    a.add<unsigned>("width", 'w', "width", true);
    a.add<unsigned>("symmetry", 's', "symmetry", true);
    a.add<bool>("ground", 'g', "ground", true);
    a.add<unsigned>("N", 'N', "N", true);
    a.add<string>("name", 'n', "name", true);

    a.parse_check(argc, argv);


    bool periodic_input = a.get<bool>("periodic_input");
    bool periodic_output = a.get<bool>("periodic_output");
    unsigned height = a.get<unsigned>("height");
    unsigned width = a.get<unsigned>("width");
    unsigned symmetry = a.get<unsigned>("symmetry");
    bool ground = a.get<bool>("ground");
    unsigned N = a.get<unsigned>("N");
    string name = a.get<std::string>("name");


    cout << "periodic_input  : " << a.get<bool>("periodic_input") << endl
         << "periodic_output : " << a.get<bool>("periodic_output") << endl
         << "height          : " << a.get<unsigned>("height") << endl
         << "width           : " << a.get<unsigned>("width") << endl
         << "symmetry        : " << a.get<unsigned>("symmetry") << endl
         << "ground          : " << a.get<bool>("ground") << endl
         << "N               : " << a.get<unsigned>("N") << endl
         << "name            : " << a.get<string>("name") << endl;

    single_run(periodic_input, periodic_output, height, width, symmetry, ground, N, name);
    return 0;
}

