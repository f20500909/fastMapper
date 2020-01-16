#ifndef FAST_WFC_UTILS_IMAGE_HPP_
#define FAST_WFC_UTILS_IMAGE_HPP_

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

//#include "include/stb_image.h"
//#include "include/stb_image_write.h"

//#include "Matrix.hpp"
//#include "color.hpp"

//std::optional<Matrix<Color>> read_image(const std::string &file_path) noexcept {
//    int width;
//    int height;
//    int num_components;
//    unsigned char *data = stbi_load(file_path.c_str(), &width, &height, &num_components, 3);
//    if (data == nullptr) {
//        return std::nullopt;
//    }
//    Matrix<Color> m = Matrix<Color>(height, width);
//    for (unsigned i = 0; i < (unsigned) height; i++) {
//        for (unsigned j = 0; j < (unsigned) width; j++) {
//            unsigned index = 3 * (i * width + j);
//            m.data[i * width + j] = Color(data[index], data[index + 1], data[index + 2]);
//        }
//    }
//    free(data);
//    return m;
//}




#endif // FAST_WFC_UTILS_IMAGE_HPP_
