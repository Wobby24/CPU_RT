#include "CPU_Raytracer.hpp"
#include "util/render_recovery.hpp"
#include "renderer/integrator.hpp"
#include "geometry/hittable_list.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"
#include "renderer/tile.hpp"
#include "util/getNextValidPPMLine.hpp"

namespace Aero::RT {
    Aero_CPU_RT::Aero_CPU_RT() {}

    Aero_CPU_RT::~Aero_CPU_RT() {}

    void Aero_CPU_RT::render(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx) {

            Utils::thread_pool thread_pool(ctx.threadCount);
            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                framebuffer.assign(ctx.width * ctx.height, 0);
            }
 
            ppmFramebuffer.assign(ctx.width * ctx.height, Utils::color(0, 0, 0));

            std::vector<Renderer::tile> tiles = Renderer::dispatchTiles(ctx.width, ctx.height, ctx.tile_size);

            isRendering = true;
            completedTileCount = recoveryIndex(ctx);
            isFileWriterRunning = true;

           // Spin up the dedicated file writing background thread
            fileWriterThread = std::thread(&Aero_CPU_RT::fileWriterLoop, this, std::ref(tiles), std::ref(ctx));


            if (ctx.multithreaded) {
                for (int k = completedTileCount; k < tiles.size(); k++) {
                        thread_pool.Enqueue([this, k, &tiles, &cam, &world, &ctx]() {
                            this->renderTiles(tiles[k], cam, world, ctx, this->framebuffer, this->ppmFramebuffer);
                            });
                }
            }
            else {
                for (int k = completedTileCount; k < tiles.size(); k++) {
                    std::jthread  thread([this, &tiles, k, cam, &world, &ctx]() {
                        renderTiles(
                            tiles[k],
                            cam,
                            world,
                            ctx,
                            this->framebuffer,
                            this->ppmFramebuffer
                        );
                    });
                }
            }

            writeToPPM(ctx);

            // check if we need to abort mid-render
            while (completedTileCount.load() < tiles.size()) {
                if (!isRendering) {
                    std::clog << "\n[Render] Abort caught in main loop. Shaking out background threads...\n";
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            isFileWriterRunning = false;

            // safe to join because the loop condition has been flipped to false
            if (fileWriterThread.joinable()) {
                fileWriterThread.join();
            }

            isRendering = false;

            std::clog << "\nRender finished. Program may now be closed." << "\n";
    }

    void Aero_CPU_RT::renderNew(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx) {
        std::clog << "\nRendering starting resolution: " << ctx.width << "x" << ctx.height << "\n";

        std::ofstream file(ctx.fileName);
        if (!file) {
            std::clog << "Could not open output file stream for " << ctx.fileName << "\n";
            return;
        }

        file << "P3\n" << ctx.width << " " << ctx.height << "\n255\n";

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
                  //  PPMframebuffer[j * ctx.width + i] = rowBuffer[i];
                    framebuffer[j * ctx.width + i] = rowBuffer[i];
                }
            }
        }
        file.close();
        std::clog << "\rProgram can now be safely closed via 'ESC' key or closing out of the window.\n";
    }

    // adapt this function for tiles
    void Aero_CPU_RT::resumeRender(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx) {

        int validPixelCount = 0;
        std::vector<uint32_t> tempBuffer(ctx.width * ctx.height, 0);

        std::ifstream readFile(ctx.fileName);
        if (readFile.is_open()) {
            std::string line;

            // 1. Safely skip/read the 3 header components using the stream helper
            Utils::getNextValidPPMLine(readFile); // Magic number ("P3")
            Utils::getNextValidPPMLine(readFile); // Dimensions ("width height")
            Utils::getNextValidPPMLine(readFile); // Max color value ("255")

            // 2. Drive the loop by assignment. This gets the next non-comment line automatically.
            while (!(line = Utils::getNextValidPPMLine(readFile)).empty()) {

                if (validPixelCount >= ctx.width * ctx.height) break;

                std::stringstream ss(line);
                uint32_t file_r, file_g, file_b;

                if (ss >> file_r >> file_g >> file_b) {
                    double r = file_r / 255.0;
                    double g = file_g / 255.0;
                    double b = file_b / 255.0;

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

        int j = validPixelCount / ctx.width;

        if (j >= ctx.height) {
            std::clog << "\nRender was already fully complete.\n";
            return;
        }

        std::clog << "\nResuming render at scanline row: " << j << "\n";

        std::ofstream file(ctx.fileName, std::ios_base::app);

        for (; j < ctx.height; j++) {
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
        std::clog << "\nRendering finished. Program can now be safely closed via 'ESC' key or closing out of the window.\n";
    }

    void Aero_CPU_RT::renderTiles(const Aero::RT::Renderer::tile& t, const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx,
        std::vector<uint32_t>& framebuffer,
        std::vector<Utils::color>& ppmFramebuffer) {

        for (int j = t.y_start; j < t.y_end; j++) {

                if (!isRendering)
                    return;

                for (int i = t.x_start; i < t.x_end; i++) {
                    Aero::RT::Utils::color pixel_color(0, 0, 0);

                    for (int s = 0; s < ctx.samples_per_pixel; s++) {
                        auto r = cam.get_ray(i, j);
                        pixel_color += Renderer::ray_color(r, ctx.max_depth, world, ctx.background);
                    }

                    pixel_color *= (1.0 / ctx.samples_per_pixel);

                    double r = Aero::RT::Utils::linear_to_gamma(pixel_color.x());
                    double g = Aero::RT::Utils::linear_to_gamma(pixel_color.y());
                    double b = Aero::RT::Utils::linear_to_gamma(pixel_color.z());

                    size_t pixelIdx = j * ctx.width + i;

                    framebuffer[pixelIdx] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
                    ppmFramebuffer[pixelIdx] = pixel_color;
                }
            }

        
            completedTileCount++;
        }

    void Aero_CPU_RT::fileWriterLoop(const std::vector<Renderer::tile>& tiles, const renderOptions& ctx) {
        std::ofstream sidecard(ctx.fileName + ".arp", std::ios_base::app);
        std::ofstream framebufferDump(ctx.fileName + ".afd", std::ios_base::app);

        int lastWrittenTileCount = 0;

        while (isFileWriterRunning) {
            int currentProgress = completedTileCount.load();

            // If the counter went up, new data is ready in the buffer
            if (currentProgress > lastWrittenTileCount) {

                // Look at which tiles are newly finished and write them
                for (int k = lastWrittenTileCount; k < currentProgress; k++) {
                    const auto& tile = tiles[k];

                    // Read directly from the buffers. This is safe because 
                    // the worker thread has completely finished modifying this region  
                    for (int j = tile.y_start; j < tile.y_end; j++) {
                        for (int i = tile.x_start; i < tile.x_end; i++) {
                            size_t idx = j * ctx.width + i;
                            framebufferDump << idx << " " << ppmFramebuffer[idx].x() << " "
                                << ppmFramebuffer[idx].y() << " " << ppmFramebuffer[idx].z() << "\n";
                        }
                    }
                    sidecard << k << "\n";
                }

                framebufferDump.flush();
                sidecard.flush();
                lastWrittenTileCount = currentProgress;
            }


            // Sleep for a tiny bit (e.g., 50 milliseconds) so this loop 
            // doesn't max out a CPU core checking the counter.
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void Aero_CPU_RT::writeToPPM(const renderOptions& ctx) {
        std::ofstream ppmFile(ctx.fileName);
        ppmFile.flush();

        ppmFile << "P3\n" << ctx.width << " " << ctx.height << "\n255\n";

        std::clog << "Finished rendering, writing image data to file..." << "\n";
        for (int y = 0; y < ctx.height; y++) {
            for (int x = 0; x < ctx.width; x++) {
                Aero::RT::Utils::write_color(ppmFile, ppmFramebuffer[y * ctx.width + x]);
            }
        }
    }

    int Aero_CPU_RT::recoveryIndex(const renderOptions& ctx) {
        // 3. RECOVERY SYSTEM: Find out where we left off
        int startingTileIdx = 0;
        std::ifstream renderProgressFile(ctx.fileName + ".arp");
        if (renderProgressFile.is_open()) {
            std::string line;
            while (std::getline(renderProgressFile, line)) {
                if (!line.empty()) {
                    startingTileIdx = std::stoi(line) + 1; // Start at the NEXT tile
                }
            }
            renderProgressFile.close();
        }

        // 4. If resuming, pull historical float data from the afd dump back into memory
        if (startingTileIdx > 0) {
            std::clog << "\nResuming tile render starting from tile index: " << startingTileIdx << "\n";
            std::ifstream framebufferDumpRead(ctx.fileName + ".afd");
            if (framebufferDumpRead.is_open()) {
                std::string line;
                while (std::getline(framebufferDumpRead, line)) {
                    if (line.empty()) continue;
                    std::stringstream ss(line);
                    size_t index;
                    double r, g, b;
                    if (ss >> index >> r >> g >> b) {
                        if (index < ppmFramebuffer.size()) {
                            ppmFramebuffer[index] = Utils::color(r, g, b);

                            double dr = Aero::RT::Utils::linear_to_gamma(r);
                            double dg = Aero::RT::Utils::linear_to_gamma(g);
                            double db = Aero::RT::Utils::linear_to_gamma(b);
                            framebuffer[index] = Aero::RT::Utils::toRGBA8(dr, dg, db, 1.0f);
                        }
                    }
                }
                framebufferDumpRead.close();
            }
        }
        else {
            std::clog << "\nStarting new render with resolution: " << ctx.width << "x" << ctx.height << "\n";
            std::ofstream clearDump(ctx.fileName + ".afd");
            std::ofstream clearArp(ctx.fileName + ".arp");
        }
        
        return startingTileIdx;
    }

      /*
        std::clog << "Finished rendering, writing image data to file..." << "\n";
        for (int y = 0; y < ctx.height; y++) {
            for (int x = 0; x < ctx.width; x++) {
                Aero::RT::Utils::write_color(file, PPMframebuffer[y * ctx.width + x]);
            }
        }

        std::cout << "Finished writing data to PPM file. Program can now be safely closed via 'ESC' key or closing out of the window.";*/  
    }

  
 /*  void Aero_CPU_RT::resumeRenderTiles(const Aero::RT::Camera::camera& cam,
        const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx) {

        std::vector<Renderer::tile> tiles = Renderer::dispatchTiles(ctx.width, ctx.height, ctx.tile_size);

        std::vector<Utils::color> localPPMBuffer(ctx.width * ctx.height, Utils::color(0, 0, 0));
        std::vector<uint32_t> localDisplayBuffer(ctx.width * ctx.height, 0);

        // 1. Correctly read the last fully completed tile index
        int tileIndex = 0;
        {
            std::ifstream sidecardRead(ctx.fileName + ".arp");
            std::string line;
            if (sidecardRead.is_open()) {
                while (std::getline(sidecardRead, line)) {
                    if (!line.empty()) tileIndex = std::stoi(line);
                }
            }
        }

        // 2. Open dump file for READING and parsing securely line-by-line
        std::ifstream framebufferDumpRead(ctx.fileName + ".afd");
        size_t pixelsRecoveredCount = 0;

        if (framebufferDumpRead.is_open()) {
            std::string line;
            while (std::getline(framebufferDumpRead, line)) {
                if (line.empty()) continue;

                std::stringstream ss(line);
                size_t index;
                double raw_r, raw_g, raw_b;

                if (ss >> index >> raw_r >> raw_g >> raw_b) {
                    if (index >= localDisplayBuffer.size()) continue;

                    // Clean up potential invalid mathematical floats
                    if (std::isnan(raw_r) || std::isinf(raw_r)) raw_r = 0.0;
                    if (std::isnan(raw_g) || std::isinf(raw_g)) raw_g = 0.0;
                    if (std::isnan(raw_b) || std::isinf(raw_b)) raw_b = 0.0;

                    localPPMBuffer[index] = Utils::color(raw_r, raw_g, raw_b);

                    double display_r = Aero::RT::Utils::linear_to_gamma(raw_r);
                    double display_g = Aero::RT::Utils::linear_to_gamma(raw_g);
                    double display_b = Aero::RT::Utils::linear_to_gamma(raw_b);

                    localDisplayBuffer[index] = Aero::RT::Utils::toRGBA8(display_r, display_g, display_b, 1.0f);
                    pixelsRecoveredCount++;
                }
            }
            framebufferDumpRead.close();
        }

      //  std::clog << "\n[Recovery] Successfully pulled " << pixelsRecoveredCount << " pixels out of cache.\n";

        // 3. Update the global display buffer all at once under lock
        {
            std::lock_guard<std::mutex> lock(framebufferMutex);
            framebuffer = localDisplayBuffer;
        }

        // 4. Open tracking streams in APPEND mode to add onto current history files
        std::ofstream sidecard(ctx.fileName + ".arp", std::ios_base::app);
        std::ofstream framebufferDump(ctx.fileName + ".afd", std::ios_base::app);

        std::clog << "Resuming tile render starting from tile index : " << tileIndex << "\n";

        // 5. Execute core loop, safely skipping finished tiles
        for (int k = 0; k < tiles.size(); k++) {

            if (k < tileIndex) continue; // Skip completed tiles

            for (int j = tiles[k].y_start; j < tiles[k].y_end; j++) {
                if (!isRendering) return;

                for (int i = tiles[k].x_start; i < tiles[k].x_end; i++) {
                    Aero::RT::Utils::color pixel_color(0, 0, 0);

                    for (int s = 0; s < ctx.samples_per_pixel; s++) {
                        auto r = cam.get_ray(i, j);
                        pixel_color += Renderer::ray_color(r, ctx.max_depth, world, ctx.background);
                    }

                    pixel_color *= (1.0 / ctx.samples_per_pixel);

                    double r = Aero::RT::Utils::linear_to_gamma(pixel_color.x());
                    double g = Aero::RT::Utils::linear_to_gamma(pixel_color.y());
                    double b = Aero::RT::Utils::linear_to_gamma(pixel_color.z());

                    size_t pixelIdx = j * ctx.width + i;

                    {
                        std::lock_guard<std::mutex> lock(framebufferMutex);
                        framebuffer[pixelIdx] = Aero::RT::Utils::toRGBA8(r, g, b, 1.0f);
                    }

                    localPPMBuffer[pixelIdx] = pixel_color;
                    framebufferDump << pixelIdx << " " << pixel_color.x() << " " << pixel_color.y() << " " << pixel_color.z() << "\n";
                }
            }
            sidecard << k << "\n";
            framebufferDump.flush();
            sidecard.flush();
        }

        // 6. Overwrite the final output image with complete assembled progress
        std::ofstream ppmFile(ctx.fileName);
        if (ppmFile) {
            ppmFile << "P3\n" << ctx.width << " " << ctx.height << "\n255\n";
            for (int y = 0; y < ctx.height; y++) {
                for (int x = 0; x < ctx.width; x++) {
                    Aero::RT::Utils::write_color(ppmFile, localPPMBuffer[y * ctx.width + x]);
                }
            }
        }
    }*/
