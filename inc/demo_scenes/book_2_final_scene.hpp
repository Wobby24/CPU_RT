#pragma once

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "util/common.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/bvh.hpp"
#include "geometry/translate.hpp"
#include "geometry/sphere.hpp" 
#include "materials/lambertian.hpp"
#include "materials/diffuse_light.hpp"
#include "geometry/constant_medium.hpp"
#include "camera/camera.hpp"


namespace Aero::RT::Demo_Scenes {
    void book_2_final_scene(Geometry::hittable_list& world, Camera::camera& cam, renderOptions& ctx, bool HQRender = false) {
        Geometry::hittable_list boxes1;
        auto ground = std::make_shared<Materials::lambertian>(Utils::color(0.48, 0.83, 0.53));
        
        int boxes_per_side = 20;
        for (int i = 0; i < boxes_per_side; i++) {
            for (int j = 0; j < boxes_per_side; j++) {
                auto w = 100.0;
                auto x0 = -1000.0 + i * w;
                auto z0 = -1000.0 + j * w;
                auto y0 = 0.0;
                auto x1 = x0 + w;
                auto y1 = random_value<double>(1, 101);
                auto z1 = z0 + w;

                boxes1.add(Geometry::box(Math::point3(x0, y0, z0), Math::point3(x1, y1, z1), ground));
            }
        }

        for (const auto& obj : boxes1.objects) {
            world.add(obj);
        }

        auto light = std::make_shared<Materials::diffuse_light>(Utils::color(7, 7, 7));
        world.add(std::make_shared<Geometry::quad>(Math::point3(123, 554, 147), Math::vec3<double>(300, 0, 0), Math::vec3<double>(0, 0, 265), light));

        auto center1 = Math::point3(400, 400, 200);
        auto center2 = center1 + Math::vec3<double>(30, 0, 0);
        auto sphere_material = std::make_shared<Materials::lambertian>(Utils::color(0.7, 0.3, 0.1));
        world.add(std::make_shared<Geometry::sphere>(center1, center2, 50, sphere_material));

        world.add(std::make_shared<Geometry::sphere>(Math::point3(260, 150, 45), 50, std::make_shared<Materials::dielectric>(1.5)));
        world.add(std::make_shared<Geometry::sphere>(
            Math::point3(0, 150, 145), 50, std::make_shared<Materials::metal>(Utils::color(0.8, 0.8, 0.9), 1.0)
        ));

        auto boundary = std::make_shared<Geometry::sphere>(Math::point3(360, 150, 145), 70, std::make_shared<Materials::dielectric>(1.5));
        world.add(boundary);
        world.add(std::make_shared<Geometry::constant_medium>(boundary, 0.2, Utils::color(0.2, 0.4, 0.9)));
        boundary = std::make_shared<Geometry::sphere>(Math::point3(0, 0, 0), 5000, std::make_shared<Materials::dielectric>(1.5));
        world.add(std::make_shared<Geometry::constant_medium>(boundary, .0001, Utils::color(1, 1, 1)));

        auto emat = std::make_shared<Materials::lambertian>(std::make_shared<Textures::image_texture>("res/textures/earthmap.jpg"));
        world.add(std::make_shared<Geometry::sphere>(Math::point3(400, 200, 400), 100, emat));
        auto pertext = std::make_shared<Textures::noise_texture>(0.2);
        world.add(std::make_shared<Geometry::sphere>(Math::point3(220, 280, 300), 80, std::make_shared<Materials::lambertian>(pertext)));

        Geometry::hittable_list boxes2;
        auto white = std::make_shared<Materials::lambertian>(Utils::color(.73, .73, .73));
        int ns = 1000;
        for (int j = 0; j < ns; j++) {
            boxes2.add(std::make_shared<Geometry::sphere>(Math::point3::random(0, 165), 10, white));
        }

        world.add(
            std::make_shared<Geometry::translate>(
                std::make_shared<Geometry::rotate_y>(
                    std::make_shared<Geometry::hittable_list>(boxes2),
                    15
                ),
                Math::vec3<double>(-100, 270, 395)
            )
        );

        world = Geometry::hittable_list(
            std::make_shared<Geometry::bvh_node>(world)
        );

        if (HQRender) {
            ctx.width = 800;
            ctx.height = 800;
            ctx.samples_per_pixel = 10000;
            ctx.max_depth = 40;
            ctx.background = Utils::color(0, 0, 0);
        }
        else {
            ctx.width = 400;
            ctx.height = 400;
            ctx.samples_per_pixel = 250;
            ctx.max_depth = 4;
            ctx.background = Utils::color(0, 0, 0);
        }

        cam.set_fov(40);
        cam.set_lookfrom(Math::point3(478, 278, -600));
        cam.set_lookat(Math::point3(278, 278, 0));
        cam.set_vup(Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0);
    }
}
