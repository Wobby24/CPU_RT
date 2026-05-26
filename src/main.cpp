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
#include "demo_scenes/bouncing_spheres.hpp"
#include "demo_scenes/checkered_spheres.hpp"
#include "camera/camera.hpp"
#include "renderer/integrator.hpp"
#include "OGL_RT_TextureView.hpp"


// abstract this stuff eventually to the render and thread pools
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

    // file null check done in trace function
    std::ofstream outFile(fileName);

    Aero::RT::Geometry::hittable_list world;

    Aero::RT::Camera::camera cam;
   
    switch (2) {
    case 1: Aero::RT::Demo_Scenes::bouncing_spheres(world, cam);  break;
    case 2: Aero::RT::Demo_Scenes::checkered_spheres(world, cam); break;
    }

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