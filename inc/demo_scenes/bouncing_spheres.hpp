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
    void bouncing_spheres(Geometry::hittable_list& world, Camera::camera& cam) {
        auto checker = std::make_shared<Aero::RT::Textures::checker_texture>(0.32, Aero::RT::Utils::color(.2, .3, .1), Aero::RT::Utils::color(.9, .9, .9));
        world.add(std::make_shared<Aero::RT::Geometry::sphere>(Aero::RT::Math::point3(0, -1000, 0), 1000, std::make_shared<Aero::RT::Materials::lambertian>(checker)));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = random_value<double>();
                Aero::RT::Math::point3 center(a + 0.9 * random_value<double>(), 0.2, b + 0.9 * random_value<double>());

                if ((center - Aero::RT::Math::point3(4, 0.2, 0)).length() > 0.9) {
                    std::shared_ptr<Aero::RT::Materials::material> sphere_material;

                    if (choose_mat < 0.8) {
                        // Diffuse
                        auto albedo = Aero::RT::Utils::color::random() * Aero::RT::Utils::color::random();
                        sphere_material = std::make_shared<Aero::RT::Materials::lambertian>(albedo);
                        auto center2 = center + Aero::RT::Math::vec3<double>(0, random_value<double>(0, .5), 0);
                        world.add(std::make_shared<Aero::RT::Geometry::sphere>(center, center2, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95) {
                        // Metal
                        auto albedo = Aero::RT::Utils::color::random(0.5, 1);
                        auto fuzz = random_value<double>(0, 0.5);
                        sphere_material = std::make_shared<Aero::RT::Materials::metal>(albedo, fuzz);
                        world.add(std::make_shared<Aero::RT::Geometry::sphere>(center, 0.2, sphere_material));
                    }
                    else {
                        // Glass
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

        world = Aero::RT::Geometry::hittable_list(std::make_shared<Aero::RT::Geometry::bvh_node>(world));

        cam.set_image_width(400);
        cam.set_samples_per_pixel(100);
        cam.set_max_depth(50);

        cam.set_fov(20);
        cam.set_lookfrom(Aero::RT::Math::point3(13, 2, 3));
        cam.set_lookat(Aero::RT::Math::point3(0, 0, 0));
        cam.set_vup(Aero::RT::Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0.6);
        cam.set_focus_distance(10.0);
    }
}