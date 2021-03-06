#ifndef SRC_FASTMAPPER_HPP
#define SRC_FASTMAPPER_HPP

#include "wfc.hpp"
#include "imageModel.hpp"
//#include "svg.hpp"

using namespace std;

bool single_run(unsigned out_height,
                unsigned out_width,
                unsigned symmetry,
                unsigned N,
                int channels,
                int log,
                string input_data,
                string output_data,
                string type) {
    srand((unsigned) time(NULL));

//    input_data = "../samples/ai/wh1.svg";
//    type = "svg";

    conf = new Config(out_height, out_width, symmetry, N, channels, log, input_data, output_data, type);

    Img<int, AbstractFeature> data ;

    data.run();
    return true;
}


#endif
