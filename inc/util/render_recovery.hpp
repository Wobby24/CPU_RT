#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Aero::RT::Utils {
    static int recover_ppm_render(const std::string& filename, int width, int height, int tile_size) {
        std::ifstream file(filename);

        if (!file.good()) {
            std::ofstream out_file(filename);
            out_file << "P3\n" << width << ' ' << height << "\n255\n";
            out_file.close();

            std::clog << "\nNo existing render found. Starting fresh!\n";
            return 0; // 0 means start at tiles[0]
        }

        std::string dummy;
        int line_count = 0;
        while (std::getline(file, dummy)) {
            if (!dummy.empty()) line_count++;
        }
        file.close();

        int pixel_lines = line_count - 3; // Subtract PPM header lines

        // Calculate how many total individual pixels are saved on disk
        int total_pixels_saved = pixel_lines;

        // Calculate the area of one standard tile (e.g., 16 * 16 = 256)
        int pixels_per_tile = tile_size * tile_size;

        // The number of completely finished tiles is our resume index!
        int completed_tiles = total_pixels_saved / pixels_per_tile;

        return completed_tiles;
    }
}