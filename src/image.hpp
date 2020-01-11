#ifndef FAST_WFC_UTILS_IMAGE_HPP_
#define FAST_WFC_UTILS_IMAGE_HPP_

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

#include "Data.hpp"
#include "color.hpp"

/**
* Read an image. Returns nullopt if there was an error.
*/
std::optional<Data<Color>> read_image(const std::string &file_path) noexcept {
    int width;
    int height;
    int num_components;
    unsigned char *data = stbi_load(file_path.c_str(), &width, &height, &num_components, 3);
    if (data == nullptr) {
        return std::nullopt;
    }
    Data<Color> m = Data<Color>(height, width);
    for (unsigned i = 0; i < (unsigned) height; i++) {
        for (unsigned j = 0; j < (unsigned) width; j++) {
            unsigned index = 3 * (i * width + j);
//			m.data[i * width + j] = { data[index], data[index + 1], data[index + 2] };
            m.data[i * width + j] = Color(data[index], data[index + 1], data[index + 2]);
//            m.data.push_back(Color(data));
        }
    }
    free(data);
    return m;
}

/**
* Write an image in the png format.
*/
void write_image_png(const std::string &file_path, const Data<Color> &m) noexcept {
    stbi_write_png(file_path.c_str(), m.width, m.height, 3, (const unsigned char *) m.data.data(), 0);
}

#endif // FAST_WFC_UTILS_IMAGE_HPP_
