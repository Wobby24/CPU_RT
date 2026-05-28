#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "util/common.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/sphere.hpp" 
#include "geometry/bvh.hpp"
#include "materials/metal.hpp"
#include "materials/dielectric.hpp"
#include "materials/lambertian.hpp"
#include "textures/checker_texture.hpp"
#include "camera/camera.hpp"

namespace Aero::RT::Demo_Scenes {
	void checkered_spheres(Geometry::hittable_list& world, Camera::camera& cam) {
        auto checker = std::make_shared<Textures::checker_texture>(0.32, Utils::color(.2, .3, .1), Utils::color(.9, .9, .9));

        world.add(std::make_shared<Geometry::sphere>(Math::point3(0, -10, 0), 10, std::make_shared<Materials::lambertian>(checker)));
        world.add(std::make_shared<Geometry::sphere>(Math::point3(0, 10, 0), 10, std::make_shared<Materials::lambertian>(checker)));

        cam.set_aspect_ratio(16.0 / 9.0);
        cam.set_image_width(400);
        cam.set_samples_per_pixel(100);
        cam.set_max_depth(50);
        cam.set_background_color(Utils::color(0.70, 0.80, 1.00));

        cam.set_fov(20);
        cam.set_lookfrom(Math::point3(13, 2, 3));
        cam.set_lookat(Math::point3(0 ,0 ,0));
        cam.set_vup(Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0);
	}
}