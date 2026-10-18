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
#include "scene/scene.hpp"
#include "scene/sceneObject.hpp"
#include <util/ArenaAllocator.hpp>


namespace Aero::RT::Demo_Scenes {
    // allocate the scene object and camera, then integrate scene parameter rendering inside the renderer, and test it.
    // this should make it way more streamline for rendering scenes
    void sceneTestScene(Scene::scene& scene, Aero::RT::renderOptions& ctx) {

        // 1. define geo and mat
        auto difflight = std::make_shared<Materials::diffuse_light>(Utils::color(4, 4, 4));
        auto pertext = std::make_shared<Textures::noise_texture>(4);

        // 2. create sceneObj
        Scene::sceneObject lightSphere(std::make_shared<Geometry::sphere>(Math::point3(0, 7, 0), 2, difflight), difflight);
        Scene::sceneObject largeSphere(std::make_shared<Geometry::sphere>(Math::point3(0, -1000, 0), 1000, std::make_shared<Materials::lambertian>(pertext)));
        Scene::sceneObject otherSphere(std::make_shared<Geometry::sphere>(Math::point3(0, 2, 0), 2, std::make_shared<Materials::lambertian>(pertext)));

        Scene::sceneObject lightQuad(std::make_shared<Geometry::quad>(Math::point3(3, 1, -2), Math::vec3<double>(2, 0, 0), Math::vec3<double>(0, 2, 0), difflight), difflight);

        scene.add_object(&lightSphere);
        scene.add_object(&largeSphere);
        scene.add_object(&otherSphere);
        scene.add_object(&lightQuad);

        ctx.width = 400;
        ctx.height = 225;
        ctx.samples_per_pixel = 100;
        ctx.max_depth = 50;
        ctx.background = Aero::RT::Utils::color(0, 0, 0);

        scene.getCurrentCam().set_fov(20);
        scene.getCurrentCam().set_lookfrom(Aero::RT::Math::point3(23, 3, 6));
        scene.getCurrentCam().set_lookat(Aero::RT::Math::point3(0, 2, 0));
        scene.getCurrentCam().set_vup(Aero::RT::Math::vec3<double>(0, 1, 0));

        scene.getCurrentCam().set_defocus_angle(0);
    }
}
