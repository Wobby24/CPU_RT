#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "util/common.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/quad.hpp" 
#include "geometry/triangle.hpp"
#include "geometry/bvh.hpp"
#include "textures/noise_texture.hpp"
#include "materials/lambertian.hpp"
#include "camera/camera.hpp"

namespace Aero::RT::Demo_Scenes {
    void quads(Geometry::hittable_list& world, Camera::camera& cam) {
        // Materials
        auto left_red     = std::make_shared<Materials::lambertian>(Utils::color(1.0, 0.2, 0.2));
        auto back_green   = std::make_shared<Materials::lambertian>(Utils::color(0.2, 1.0, 0.2));
        auto right_blue   = std::make_shared<Materials::lambertian>(Utils::color(0.2, 0.2, 1.0));
        auto upper_orange = std::make_shared<Materials::lambertian>(Utils::color(1.0, 0.5, 0.0));
        auto lower_teal   = std::make_shared<Materials::lambertian>(Utils::color(0.2, 0.8, 0.8));

        // Quads
        world.add(std::make_shared<Geometry::quad>(Math::point3(-3,-2, 5), Math::vec3<double>(0, 0,-4), Math::vec3<double>(0, 4, 0), left_red));
        world.add(std::make_shared<Geometry::quad>(Math::point3(-2,-2, 0), Math::vec3<double>(4, 0, 0), Math::vec3<double>(0, 4, 0), back_green));
        world.add(std::make_shared<Geometry::quad>(Math::point3( 3,-2, 1), Math::vec3<double>(0, 0, 4), Math::vec3<double>(0, 4, 0), right_blue));
        world.add(std::make_shared<Geometry::quad>(Math::point3(-2, 3, 1), Math::vec3<double>(4, 0, 0), Math::vec3<double>(0, 0, 4), upper_orange));
        world.add(std::make_shared<Geometry::quad>(Math::point3(-2,-3, 5), Math::vec3<double>(4, 0, 0), Math::vec3<double>(0, 0,-4), lower_teal));


        world = Aero::RT::Geometry::hittable_list(std::make_shared<Aero::RT::Geometry::bvh_node>(world));

        cam.set_aspect_ratio(1.0);
        cam.set_image_width(400);
        cam.set_samples_per_pixel(100);
        cam.set_max_depth(50);

        cam.set_fov(80);
        cam.set_lookfrom(Math::point3(0, 0, 9));
        cam.set_lookat(Math::point3(0, 0, 0));
        cam.set_vup(Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0);
    }
}