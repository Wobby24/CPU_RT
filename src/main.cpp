#include <iostream>
#include <fstream>
#include <memory>

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/sphere.hpp" 
#include "camera/camera.hpp"
#include "camera/compute_ray_color.hpp"


void renderToPPM(Aero::RT::Camera::camera& cam, const Aero::RT::Geometry::hittable& world, std::ofstream file) {

	file << "P3\n" << cam.get_image_width() << " " << cam.get_image_height() << "\n255\n";

	for (int j = 0; j < cam.get_image_height(); j++) {
		std::clog << "\rScanlines remaining: " << (cam.get_image_height() - j) << ' ' << std::flush;
		for (int i = 0; i < cam.get_image_width(); i++) {
			Aero::RT::Utils::color pixel_color = Aero::RT::compute_ray_color(cam.get_ray(i, j), world);
			Aero::RT::Utils::write_color(file, pixel_color);
		}
	}

	std::clog << "\rDone.                 \n";
}
int main() {
	// file (move to fileio eventually)
	std::string fileName = "render.ppm";

	std::ofstream outFile(fileName);
	if (!outFile) {
		std::cerr << "Failed to open output file for writing: " << fileName << std::endl;
		return -1;
	}

	// World
	Aero::RT::Geometry::hittable_list world;

	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0, 0, -1), 0.5));
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0, -100.5, -1), 100));

	Aero::RT::Camera::camera cam;
	cam.set_samples_per_pixel(100);

	renderToPPM(cam, world, std::move(outFile));

	return 0;
}