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
#include "util/common.hpp"


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

    auto ground_material = std::make_shared<Aero::RT::Materials::lambertian>(Aero::RT::Utils::color(0.5, 0.5, 0.5));
    world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_value<double>();
            Aero::RT::Math::point3 center(a + 0.9 * random_value<double>(), 0.2, b + 0.9 * random_value<double>());

            if ((center - Aero::RT::Math::point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Aero::RT::Materials::material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Aero::RT::Utils::color::random() * Aero::RT::Utils::color::random();
                    sphere_material = std::make_shared<Aero::RT::Materials::lambertian>(albedo);
                    world.add(std::make_shared<Aero::RT::Geometry::sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Aero::RT::Utils::color::random(0.5, 1);
                    auto fuzz = random_value<double>(0, 0.5);
                    sphere_material = std::make_shared<Aero::RT::Materials::metal>(albedo, fuzz);
                    world.add(std::make_shared<Aero::RT::Geometry::sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<Aero::RT::Materials::dielectric>(1.5);
                    world.add(std::make_shared<Aero::RT::Geometry::sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Aero::RT::Materials::dielectric>(1.5);
    world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Aero::RT::Materials::lambertian>(Aero::RT::Utils::color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Aero::RT::Materials::metal>(Aero::RT::Utils::color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(4, 1, 0), 1.0, material3));

    Aero::RT::Camera::camera cam;
    cam.set_image_width(500);
    cam.set_samples_per_pixel(100);
    cam.set_max_depth(50);

    cam.set_fov(20);
    cam.set_lookfrom(Aero::RT::Math::point3(13, 2, 3));
    cam.set_lookat(Aero::RT::Math::point3(0, 0, 0));
    cam.set_vup(Aero::RT::Math::vec3<double>(0, 1, 0));

    cam.set_defocus_angle(0.6);
    cam.set_focus_distance(1.0);

	// then call init explicity
	cam.initialize();

    // //#pragma omp parallel for schedule(dynamic) idk just try it and see what it does since we arent at manual multithreaidng or pools yet
	renderToPPM(cam, world, outFile);

	return 0;
}
