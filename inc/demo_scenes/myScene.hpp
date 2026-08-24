#pragma once
#include "scene/scene.hpp"
#include "scene/sceneObject.hpp"

#include <iostream>
#include <vector>
#include <memory>

#include "math/vec3.hpp"
#include "util/color.hpp"
#include "util/common.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "geometry/bvh.hpp"
#include "geometry/translate.hpp"
#include "materials/lambertian.hpp"
#include "materials/diffuse_light.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"

namespace Aero::RT::Demo_Scenes {
    void TestScene(Camera::camera& cam, renderOptions& ctx) {
        Scene::scene scene;

        auto red = std::make_shared<Materials::lambertian>(Utils::color(.65, .05, .05));
        auto white = std::make_shared<Materials::lambertian>(Utils::color(.73, .73, .73));
        auto green = std::make_shared<Materials::lambertian>(Utils::color(.12, .45, .15));
        auto light = std::make_shared<Materials::diffuse_light>(Utils::color(15, 15, 15));

        world.add(std::make_shared<Geometry::quad>(Math::point3(555, 0, 0), Math::vec3<double>(0, 555, 0), Math::vec3<double>(0, 0, 555), red));
        world.add(std::make_shared<Geometry::quad>(Math::point3(0, 0, 0), Math::vec3<double>(0, 555, 0), Math::vec3<double>(0, 0, 555), green));
        world.add(std::make_shared<Geometry::quad>(Math::point3(343, 554, 332), Math::vec3<double>(-130, 0, 0), Math::vec3<double>(0, 0, -105), light));
        world.add(std::make_shared<Geometry::quad>(Math::point3(0, 0, 0), Math::vec3<double>(555, 0, 0), Math::vec3<double>(0, 0, 555), white));
        world.add(std::make_shared<Geometry::quad>(Math::point3(555, 555, 555), Math::vec3<double>(-555, 0, 0), Math::vec3<double>(0, 0, -555), white));
        world.add(std::make_shared<Geometry::quad>(Math::point3(0, 0, 555), Math::vec3<double>(555, 0, 0), Math::vec3<double>(0, 555, 0), white));

        Scene::sceneObject quad1(Geometry::quad(Math::point3(555, 0, 0), Math::vec3<double>(0, 555, 0), Math::vec3<double>(0, 0, 555), red))

        std::shared_ptr<Geometry::hittable> box1 = Geometry::box(Math::point3(0, 0, 0), Math::point3(165, 330, 165), white);
        box1 = std::make_shared<Geometry::rotate_y>(box1, 15);
        box1 = std::make_shared<Geometry::translate>(box1, Math::vec3<double>(265, 0, 295));
        world.add(box1);

        std::shared_ptr<Geometry::hittable> box2 = Geometry::box(Math::point3(0, 0, 0), Math::point3(165, 165, 165), white);
        box2 = std::make_shared<Geometry::rotate_y>(box2, -18);
        box2 = std::make_shared<Geometry::translate>(box2, Math::vec3<double>(130, 0, 65));
        world.add(box2);

        world = Aero::RT::Geometry::hittable_list(std::make_shared<Aero::RT::Geometry::bvh_node>(world));

        ctx.width = 600;
        ctx.height = 600;
        ctx.samples_per_pixel = 100;
        ctx.max_depth = 50;
        ctx.background = Utils::color(0, 0, 0);

        cam.set_fov(40);
        cam.set_lookfrom(Math::point3(278, 278, -800));
        cam.set_lookat(Math::point3(278, 278, 0));
        cam.set_vup(Math::vec3<double>(0, 1, 0));

        cam.set_defocus_angle(0);
    }
}