#include "CPU_Raytracer.hpp"
#include "util/render_recovery.hpp"
#include "renderer/integrator.hpp"
#include "geometry/hittable_list.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"

namespace Aero::RT {
    Aero_CPU_RT::Aero_CPU_RT() {}

    Aero_CPU_RT::~Aero_CPU_RT() {}

    void Aero_CPU_RT::render(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx) {

        // Allocate framebuffer and fill it with black in unrendered areas
            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                framebuffer.resize(ctx.width * ctx.height, 0);
            }

            int j = Utils::recover_ppm_render(ctx.fileName, ctx.width, ctx.height);

            isRendering = true;

            // In a bottom-up system, a fresh render starts at j = 0.
            // If recover_ppm_render returns anything greater than 0, we have data to resume!
            if (j > 0 && j < ctx.height) {
                resumeRender(cam, world, ctx);
            }
            else {
                renderNew(cam, world, ctx);
            }

            isRendering = false;
    }

    void Aero_CPU_RT::renderNew(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx) {
        std::clog << "\nRendering starting (Bottom-Up) resolution: " << ctx.width << "x" << ctx.height << "\n";

        std::ofstream file(ctx.fileName);
        if (!file) {
            std::clog << "Could not open output file stream for " << ctx.fileName << "\n";
            return;
        }

        file << "P3\n" << ctx.width << " " << ctx.height << "\n255\n";

        // Bottom-Up Loop: Counting UP from 0 to height
        for (int j = 0; j < ctx.height; j++) {
            if (!isRendering) break;

            std::clog << "\rScanlines completed: " << j << "/" << ctx.height << std::flush;

            std::vector<uint32_t> rowBuffer(ctx.width);

            for (int i = 0; i < ctx.width; i++) {
                Aero::RT::Utils::color pixel_color(0, 0, 0);

                for (int s = 0; s < ctx.samples_per_pixel; s++) {
                    auto r = cam.get_ray(i, j);
                    pixel_color += Renderer::ray_color(r, ctx.max_depth, world, ctx.background);
                }

                double scale = 1.0 / ctx.samples_per_pixel;
                pixel_color *= scale;
                Aero::RT::Utils::write_color(file, pixel_color);

                double r = Aero::RT::Utils::linear_to_gamma(pixel_color.x());
                double g = Aero::RT::Utils::linear_to_gamma(pixel_color.y());
                double b = Aero::RT::Utils::linear_to_gamma(pixel_color.z());

                rowBuffer[i] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
            }

            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                for (int i = 0; i < ctx.width; i++) {
                    framebuffer[j * ctx.width + i] = rowBuffer[i];
                }
            }
        }
        file.close();
        std::clog << "\rDone Tracing.                  \n";
    }

    void Aero_CPU_RT::resumeRender(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx) {
        int validPixelCount = 0;

        // Initialize our temporary buffer with RED so unrendered lines remain visible
        std::vector<uint32_t> tempBuffer(ctx.width * ctx.height, 0);

        std::ifstream readFile(ctx.fileName);
        if (readFile.is_open()) {
            std::string line;

            std::getline(readFile, line); // Skip headers
            std::getline(readFile, line);
            std::getline(readFile, line);

            while (std::getline(readFile, line)) {
                if (line.empty()) continue;
                if (validPixelCount >= ctx.width * ctx.height) break;

                std::stringstream ss(line);
                uint32_t file_r, file_g, file_b;

                if (ss >> file_r >> file_g >> file_b) {
                    double r = file_r / 255.0;
                    double g = file_g / 255.0;
                    double b = file_b / 255.0;

                    // FIX: Because our file is written bottom-up, validPixelCount 
                    // maps directly to our bottom-up framebuffer sequence! No flipping required.
                    tempBuffer[validPixelCount] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
                    validPixelCount++;
                }
            }
            readFile.close();

            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                framebuffer = tempBuffer;
            }
        }

        // Calculate how many rows are done
        int j = validPixelCount / ctx.width;

        if (j >= ctx.height) {
            std::clog << "\nRender was already fully complete.\n";
            return;
        }

        std::clog << "\nResuming bottom-up render at scanline row: " << j << "\n";

        std::ofstream file(ctx.fileName, std::ios_base::app);

        // FIX: Count UP to ctx.height to maintain perfect bottom-up consistency
        for (; j < ctx.height; j++) {
            if (!isRendering) break;

            std::clog << "\rScanlines completed: " << j << "/" << ctx.height << std::flush;

            if (!file.is_open()) {
                std::clog << "\nLost access to write stream at row: " << j << "\n";
                break;
            }

            std::vector<uint32_t> rowBuffer(ctx.width);

            for (int i = 0; i < ctx.width; i++) {
                Aero::RT::Utils::color pixel_color(0, 0, 0);

                for (int s = 0; s < ctx.samples_per_pixel; s++) {
                    auto r = cam.get_ray(i, j);
                    pixel_color += Aero::RT::Renderer::ray_color(r, ctx.max_depth, world, ctx.background);
                }

                double scale = 1.0 / ctx.samples_per_pixel;
                pixel_color *= scale;
                Aero::RT::Utils::write_color(file, pixel_color);

                double r = Aero::RT::Utils::linear_to_gamma(pixel_color.x());
                double g = Aero::RT::Utils::linear_to_gamma(pixel_color.y());
                double b = Aero::RT::Utils::linear_to_gamma(pixel_color.z());

                rowBuffer[i] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
            }

            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                for (int i = 0; i < ctx.width; i++) {
                    framebuffer[j * ctx.width + i] = rowBuffer[i];
                }
            }
        }
        file.close();
        std::clog << "\rDone Tracing.                  \n";
    }
}