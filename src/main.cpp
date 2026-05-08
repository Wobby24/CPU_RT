#include <iostream>
#include <fstream>
#include <memory>

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/sphere.hpp" 
#include "materials/metal.hpp"
#include "materials/dielectric.hpp"
#include "materials/lambertian.hpp"
#include "camera/camera.hpp"
#include "renderer/integrator.hpp"


void renderToPPM(Aero::RT::Camera::camera& cam, const Aero::RT::Geometry::hittable& world, std::ofstream& file) {

	file << "P3\n" << cam.get_image_width() << " " << cam.get_image_height() << "\n255\n";

	for (int j = 0; j < cam.get_image_height(); j++) {
		for (int i = 0; i < cam.get_image_width(); i++) {

			std::clog << "\rScanlines remaining: " << (cam.get_image_height() - j) << ' ' << std::flush;

			Aero::RT::Utils::color pixel_color(0, 0, 0);

			for (int s = 0; s < cam.get_samples_per_pixel(); s++) {
				auto r = cam.get_ray(i, j);
				pixel_color += Aero::RT::Renderer::ray_color(r, cam.get_max_depth(), world);
			}

			pixel_color *= cam.get_samples_per_pixel_scale();
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

	// define materials
	auto material_ground = std::make_shared<Aero::RT::Materials::lambertian>(Aero::RT::Utils::color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Aero::RT::Materials::lambertian>(Aero::RT::Utils::color(0.1, 0.2, 0.5));
	auto material_left = std::make_shared<Aero::RT::Materials::dielectric>(1.50);
	auto material_bubble = std::make_shared<Aero::RT::Materials::dielectric>(1.00 / 1.50);
	auto material_right = std::make_shared<Aero::RT::Materials::metal>(Aero::RT::Utils::color(0.8, 0.6, 0.2), 1.0);

	// add spheres with materials to world
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0.0, 0.0, -1.2), 0.5, material_center));
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
	world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(1.0, 0.0, -1.0), 0.5, material_right));

	Aero::RT::Camera::camera cam;
	cam.set_samples_per_pixel(100);
	cam.set_max_depth(50);

	renderToPPM(cam, world, outFile);

	return 0;
}