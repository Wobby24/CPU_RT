#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

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
#include "renderer/integrator.hpp"
#include "OGL_RT_TextureView.hpp"


std::vector<uint32_t> framebuffer;
std::mutex framebufferMutex;
std::atomic<bool> isRendering{ true };

static void raytraceWorker(
    const Aero::RT::Camera::camera& cam,
    const Aero::RT::Geometry::hittable_list& world,
    std::string fileName,
    int width,
    int height
) {
    std::clog << "\nRendering starting with resolution parameters: " << width << "x" << height << "\n";

    std::ofstream file(fileName);
    if (!file) {
        std::clog << "Could not open output file stream for " << fileName << "\n";
        return;
    }

    file << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; j++) {
        if (!isRendering) break;

        std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;

        // Temporary local row buffer to optimize lock contention
        std::vector<uint32_t> rowBuffer(width);

        for (int i = 0; i < width; i++) {
            Aero::RT::Utils::color pixel_color(0, 0, 0);

            for (int s = 0; s < cam.get_samples_per_pixel(); s++) {
                auto r = cam.get_ray(i, j);
                pixel_color += Aero::RT::Renderer::ray_color(r, cam.get_max_depth(), world);
            }

            pixel_color *= cam.get_samples_per_pixel_scale();
            Aero::RT::Utils::write_color(file, pixel_color);

            double r = Aero::RT::Utils::linear_to_gamma(pixel_color.x());
            double g = Aero::RT::Utils::linear_to_gamma(pixel_color.y());
            double b = Aero::RT::Utils::linear_to_gamma(pixel_color.z());

            rowBuffer[i] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
        }

        // Lock global memory just long enough to copy the fresh row array data safely
        {
            std::lock_guard<std::mutex> lock(framebufferMutex);
            for (int i = 0; i < width; i++) {
                framebuffer[j * width + i] = rowBuffer[i];
            }
        }
    }
    std::clog << "\rDone Tracing.                  \n";
}

int main() {
    std::string fileName = "render.ppm";

    std::ofstream outFile(fileName);
    // check done in rt function
   /*if (!outFile) {
       std::cerr << "Failed to open output file for writing: " << fileName << std::endl;
       return -1;
    }*/ 

    // World Definition
    Aero::RT::Geometry::hittable_list world;

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

    // Camera Configuration Setup
    Aero::RT::Camera::camera cam;
    cam.set_image_width(400);
    cam.set_samples_per_pixel(100);
    cam.set_max_depth(50);

    cam.set_fov(20);
    cam.set_lookfrom(Aero::RT::Math::point3(13, 2, 3));
    cam.set_lookat(Aero::RT::Math::point3(0, 0, 0));
    cam.set_vup(Aero::RT::Math::vec3<double>(0, 1, 0));

    cam.set_defocus_angle(0.6);
    cam.set_focus_distance(10.0);

    cam.initialize();

    int width = cam.get_image_width();
    int height = cam.get_image_height();

    // Resize global buffer safely matching camera dimensions
    framebuffer.resize(width * height, 0);

    Aero::GL::OGL_RT_TextureViewer texViewer;
    isRendering = true;

    std::thread tracerThread(raytraceWorker, std::ref(cam), std::ref(world), "render.ppm", width, height);

    // Interactive responsive execution window loop
    while (!texViewer.shouldWindowClose()) {

        std::vector<uint32_t> bufferCopy;

        // Isolate block loop to perform thread safe vector deep copy fast
        {
            std::lock_guard<std::mutex> lock(framebufferMutex);
            bufferCopy = framebuffer;
        }

        texViewer.renderToTexture(bufferCopy, width, height);

        // Frame limiter rest slice to avoid maxing out a full primary thread pipeline state
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    isRendering = false;
    if (tracerThread.joinable()) {
        tracerThread.join();
    }

    return 0;
}