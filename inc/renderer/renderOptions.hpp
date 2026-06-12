#pragma once

#include <string>
#include "util/color.hpp" 

namespace Aero::RT {
    struct renderOptions {
        int width = 1280;
        int height = 720;

        int samples_per_pixel = 10;
        int max_depth = 10;

        int tile_size = 16;
        int threadCount = 1;
        bool multithreaded = true;

        Utils::color background = Utils::color(0.0, 0.0, 0.0); 
        std::string fileName = "render.ppm";
    };
}