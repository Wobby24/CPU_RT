#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Aero::RT::Utils {
    static int recover_ppm_render(const std::string& filename,
        int width,
        int height)
    {
        std::ifstream file(filename);

        if (!file.good()) {
            std::ofstream out(filename);
            out << "P3\n" << width << ' ' << height << "\n255\n";
            return 0;
        }

        std::string line;
        int pixel_count = 0;

        std::getline(file, line);
        std::getline(file, line);
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (!line.empty())
                pixel_count++;
        }

        return pixel_count / width;
    }
}