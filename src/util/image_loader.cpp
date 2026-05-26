#include "util/image_loader.hpp"

// Disable strict warnings for compiling the third-party STB header
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#ifdef _MSC_VER
    #pragma warning (pop)
#endif

#include <cstdlib>
#include <iostream>


namespace Aero::RT::Utils {
    imageLoader::imageLoader() {}

imageLoader::imageLoader(const char* image_filename) {
    auto filename = std::string(image_filename);
    auto imagedir = getenv("MYPROJECT_IMAGE_DIR");

    // Hunt for the image file in some likely locations.
    if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
    if (load(filename)) return;
    if (load("images/" + filename)) return;
    if (load("../images/" + filename)) return;
    if (load("../../images/" + filename)) return;
    if (load("../../../images/" + filename)) return;
    if (load("../../../../images/" + filename)) return;
    if (load("../../../../../images/" + filename)) return;
    if (load("../../../../../../images/" + filename)) return;

    std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
}

imageLoader::~imageLoader() {
    delete[] bdata;
    STBI_FREE(fdata);
}

// --- Public Methods ---

bool imageLoader::load(const std::string& filename) {
    auto n = bytes_per_pixel; 
    fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
    if (fdata == nullptr) return false;

    bytes_per_scanline = image_width * bytes_per_pixel;
    convert_to_bytes();
    return true;
}

int imageLoader::width() const { 
    return (fdata == nullptr) ? 0 : image_width; 
}

int imageLoader::height() const { 
    return (fdata == nullptr) ? 0 : image_height; 
}

const unsigned char* imageLoader::pixel_data(int x, int y) const {
    static unsigned char magenta[] = { 255, 0, 255 };
    if (bdata == nullptr) return magenta;

    x = clamp(x, 0, image_width);
    y = clamp(y, 0, image_height);
    return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
}

// --- Private Helpers ---

int imageLoader::clamp(int x, int low, int high) {
    if (x < low) return low;
    if (x < high) return x;
    return high - 1;
}

unsigned char imageLoader::float_to_byte(float value) {
    if (value <= 0.0) return 0;
    if (1.0 <= value) return 255;
    return static_cast<unsigned char>(256.0 * value);
}

void imageLoader::convert_to_bytes() {
    int total_bytes = image_width * image_height * bytes_per_pixel;
    bdata = new unsigned char[total_bytes];

    auto *bptr = bdata;
    auto *fptr = fdata;
    for (auto i = 0; i < total_bytes; i++, fptr++, bptr++)
        *bptr = float_to_byte(*fptr);
}
}


