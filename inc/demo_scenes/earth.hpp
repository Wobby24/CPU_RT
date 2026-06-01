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
#include "textures/image_texture.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"

namespace Aero::RT::Demo_Scenes {
    void earth(Geometry::hittable_list& world, Camera::camera& cam, renderOptions& ctx) {
    auto earth_texture = std::make_shared<Textures::image_texture>("res/textures/earthmap.jpg");
    auto earth_surface = std::make_shared<Materials::lambertian>(earth_texture);
    auto globe = std::make_shared<Geometry::sphere>(Math::point3(0,0,0), 2, earth_surface);

    world.add(globe);

    world = Aero::RT::Geometry::hittable_list(std::make_shared<Aero::RT::Geometry::bvh_node>(world));

    ctx.width = 400;
    ctx.height = 225;
    ctx.samples_per_pixel = 100;
    ctx.max_depth = 50;
    ctx.background = Utils::color(0.70, 0.80, 1.00);

    cam.set_fov(20);
    cam.set_lookfrom(Math::point3(0,0,12));
    cam.set_lookat(Math::point3(0,0,0));
    cam.set_vup(Math::vec3<double>(0,1,0));

    cam.set_defocus_angle(0);
}
}
