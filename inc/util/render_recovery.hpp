#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace Aero::RT::Utils {
	static int recover_ppm_render(const std::string & filename, int width, int height) {
		std::ifstream file(filename);

        if (!file.good()) {
            std::ofstream out_file(filename);
            out_file << "P3\n" << width << ' ' << height << "\n255\n";
            out_file.close();

            std::clog << "\nNo existing render found. Starting fresh!\n";
            return height - 1;
        }

        std::string dummy;
        int line_count = 0;
        while (std::getline(file, dummy)) {
            line_count++;
        }
        file.close();

        // 3 lines for the PPM header
        int pixel_lines = line_count - 3;

        // In case the crash happened midway through writing a single scanline,
        // we only count completely finished scanlines.
        int scanlines_completed = pixel_lines / width;

        // Convert completed scanlines into the loop's 'j' index.
        // (Since j counts DOWN from height-1 to 0)
        int resume_j = (height - 1) - scanlines_completed;

        return resume_j;
	}
}