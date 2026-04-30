#include <iostream>
#include <fstream>

#include "math/vec3.hpp"
#include "math/ray.hpp"
#include "util/color.hpp"

using namespace Aero::RT;

double hit_sphere(const Aero::RT::Math::point3& center, double radius, const Aero::RT::Math::ray& r) {
	Aero::RT::Math::vec3<double> oc = center - r.origin();
	auto a = r.direction().length_squared();
	auto h = dot(r.direction(), oc); 
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = h * h - a * c;
	
	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (h- std::sqrt(discriminant)) /  * a;
	}
}

Aero::RT::Utils::color ray_color(const Aero::RT::Math::ray& r) {
	auto t = hit_sphere(Aero::RT::Math::point3(0, 0, -1), 0.5, r);
	if (t > 0.0) {
		Math::vec3<double> N = unit_vector(r.at(t) - Math::vec3<double>(0, 0, -1));
		return 0.5 * Utils::color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	Aero::RT::Math::vec3<double> unit_direction = Aero::RT::Math::unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - a) * Aero::RT::Utils::color(1.0, 1.0, 1.0) + a * Aero::RT::Utils::color(0.5, 0.7, 1.0);
}

int main() {
	 // Image parameters

	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 400;

	// Calculate the image hieght, and ensure that it's at least 1
	int image_height = int(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height;

	// Camera 

	auto focal_length = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(image_width) / image_height);
	auto camera_center = Aero::RT::Math::point3(0, 0, 0);

	// Calculate the vectors across the horizontal and own the vertical viewport edges.
	auto viewport_u = Aero::RT::Math::vec3<double>(viewport_width, 0, 0);
	auto viewport_v = Aero::RT::Math::vec3<double>(0, -viewport_height, 0);

	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	auto pixel_delta_u = viewport_u / image_width;
	auto pixel_delta_v = viewport_v / image_height;

	// Calculate the location of the upper left pixel.
	auto viewport_upper_left = camera_center
							 - Aero::RT::Math::vec3<double>(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


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
			auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_direction = pixel_center - camera_center;
			Aero::RT::Math::ray r(camera_center, ray_direction);

			Aero::RT::Utils::color pixel_color = ray_color(r);
			Aero::RT::Utils::write_color(outFile, pixel_color);
		}
	}

	std::clog << "\rDone.                 \n";

	return 0;
}