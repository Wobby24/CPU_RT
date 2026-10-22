#pragma once

#include <string>
#include <thread>
#include "util/color.hpp" 

namespace Aero::RT {
    inline uint32_t calculate_optimal_thread_usage() {
        uint32_t threadCount = std::thread::hardware_concurrency();

        if (threadCount < 4) {
            return threadCount;
        }
        else if (threadCount > 4 && threadCount < 16) {
            return threadCount - 2;
        }
        else {
            return threadCount - 4;
        }
    }

    struct renderOptions {
        int width = 1280;
        int height = 720;

        int samples_per_pixel = 10;
        int max_depth = 10;

        int tile_size = 32;
        uint32_t threadCount = calculate_optimal_thread_usage();
        bool multithreaded = false;

        Utils::color background = Utils::color(0.0, 0.0, 0.0); 
        std::string fileName = "render.ppm";
    };
}
