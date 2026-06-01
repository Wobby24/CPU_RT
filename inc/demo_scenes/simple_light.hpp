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
#include "materials/lambertian.hpp"
#include "materials/diffuse_light.hpp"
#include "textures/checker_texture.hpp"
#include "textures/noise_texture.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"

namespace Aero::RT::Demo_Scenes {
    void simple_lights(Geometry::hittable_list& world, Camera::camera& cam, renderOptions& ctx) {
        auto pertext = std::make_shared<Textures::noise_texture>(4);
        auto difflight = std::make_shared<Materials::diffuse_light>(Utils::color(4, 4, 4));
        
        world.add(std::make_shared<Geometry::sphere>(Math::point3(0, 7, 0), 2, difflight));
        world.add(std::make_shared<Geometry::sphere>(Math::point3(0, -1000, 0), 1000, std::make_shared<Materials::lambertian>(pertext)));
        world.add(std::make_shared<Geometry::sphere>(Math::point3(0, 2, 0), 2, std::make_shared<Materials::lambertian>(pertext)));

        world.add(std::make_shared<Geometry::quad>(Math::point3(3, 1, -2), Math::vec3<double>(2, 0, 0), Math::vec3<double>(0, 2, 0), difflight));

        world = Aero::RT::Geometry::hittable_list(std::make_shared<Aero::RT::Geometry::bvh_node>(world));

        ctx.width = 400;
        ctx.height = 225;
        ctx.samples_per_pixel = 100;
        ctx.max_depth = 50;
        ctx.background = Utils::color(0, 0, 0);

        cam.set_fov(20);
        cam.set_lookfrom(Math::point3(23, 3, 6));
        cam.set_lookat(Math::point3(0, 2, 0));
        cam.set_vup(Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0);
    }
}