#include <iostream>
#include <fstream>

#include "math/vec3.hpp"
#include "util/color.hpp"

int main() {
	 // Image parameters

	int image_width = 256;
	int image_height = 256;

	// Render to PPM
	// Create File
	// Add "rt_output" file to it
	std::string fileName = "test.ppm";

	std::ofstream outFile("rt_output/" + fileName);
	if (!outFile) {
		std::cerr << "Failed to open output file for writing: test.ppm" << std::endl;
	}

	outFile << "P3\n" << image_width << " " << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i = 0; i < image_width; i++) {
			auto pixel_color = Aero::RT::Utils::color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0);
			Aero::RT::Utils::write_color(outFile, pixel_color);
		}
	}

	std::clog << "\rDone.                 \n";

	return 0;
}