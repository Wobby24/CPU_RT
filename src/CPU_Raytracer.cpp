#include "CPU_Raytracer.hpp"
#include "util/render_recovery.hpp"
#include "renderer/integrator.hpp"
#include "geometry/hittable_list.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"
#include "renderer/tile.hpp"
#include "geometry/bvh.hpp"
#include "util/getNextValidPPMLine.hpp"
#include <filesystem>
#include <scene/scene.hpp>

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

            //completedTileCount = recoveryIndex(ctx);

            isRendering = true;
            isFileWriterRunning = true;

           // Spin up the dedicated file writing background thread
            fileWriterThread = std::thread(&Aero_CPU_RT::fileWriterLoop, this, std::ref(tiles), std::ref(ctx), completedTileCount.load());

            try {
                if (ctx.multithreaded) {
                    for (int k = completedTileCount; k < tiles.size(); k++) {
                        thread_pool.Enqueue([this, k, &tiles, &cam, &world, &ctx]() {
                            this->renderTiles(k, tiles[k], cam, world, ctx, this->framebuffer, this->ppmFramebuffer);
                            });
                    }
                }
                else {
                    for (int k = completedTileCount; k < tiles.size(); k++) {
                        renderTiles(k, tiles[k], cam, world, ctx, this->framebuffer, this->ppmFramebuffer);
                    }
                }
            }
            catch (std::exception& e) {
                std::cout << "Exception: " << e.what() << '\n';
            }
            catch (...) {
                std::cout << "Exception: unknown" << '\n';
            }

            // check if we need to abort mid-render
            while (completedTileCount.load() < tiles.size()) {
                if (!isRendering) {
                    std::clog << "\nAbort caught in main loop. Attempting to close program...\n";
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

            writeToPPM(ctx);


            std::clog << "\nRender finished. Program may now be closed." << "\n";
    }

    void Aero_CPU_RT::render(Scene::scene& scene, const renderOptions& ctx) {

            Utils::thread_pool thread_pool(ctx.threadCount);
            {
                std::lock_guard<std::mutex> lock(framebufferMutex);
                framebuffer.assign(ctx.width * ctx.height, 0);
            }

            ppmFramebuffer.assign(ctx.width * ctx.height, Utils::color(0, 0, 0));

            std::vector<Renderer::tile> tiles = Renderer::dispatchTiles(ctx.width, ctx.height, ctx.tile_size);

            //completedTileCount = recoveryIndex(ctx);

            isRendering = true;
            isFileWriterRunning = true;

           // Spin up the dedicated file writing background thread
            fileWriterThread = std::thread(&Aero_CPU_RT::fileWriterLoop, this, std::ref(tiles), std::ref(ctx), completedTileCount.load());

            Aero::RT::Geometry::hittable_list world = convertSceneObjsGeo(scene);
            Aero::RT::Camera::camera cam = scene.getSceneCam(0);
            try {
                if (ctx.multithreaded) {
                    for (int k = completedTileCount; k < tiles.size(); k++) {
                        thread_pool.Enqueue([this, k, &tiles, &cam, &world, &ctx]() {
                            this->renderTiles(k, tiles[k], cam, world, ctx, this->framebuffer, this->ppmFramebuffer);
                            });
                    }
                }
                else {
                    for (int k = completedTileCount; k < tiles.size(); k++) {
                        renderTiles(k, tiles[k], cam, world, ctx, this->framebuffer, this->ppmFramebuffer);
                    }
                }
            }
            catch (std::exception& e) {
                std::cout << "Exception: " << e.what() << '\n';
            }
            catch (...) {
                std::cout << "Exception: unknown" << '\n';
            }

            // check if we need to abort mid-render
            while (completedTileCount.load() < tiles.size()) {
                if (!isRendering) {
                    std::clog << "\nAbort caught in main loop. Attempting to close program...\n";
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

            writeToPPM(ctx);


            std::clog << "\nRender finished. Program may now be closed." << "\n";
    }

    void Aero_CPU_RT::renderTiles(int tileIndex, const Aero::RT::Renderer::tile& t, const Aero::RT::Camera::camera& cam,
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

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            finishedTileQueue.push(tileIndex);
        }
        queueCV.notify_one(); // Wake up the file writer
    }

    Aero::RT::Geometry::hittable_list Aero_CPU_RT::convertSceneObjsGeo(Scene::scene& scene) {
        std::cout << "1. Starting scene conversion\n";

        Aero::RT::Geometry::hittable_list sceneWorld;

        for (auto& sceneObject : scene.getSceneObjs()) {
            std::cout << "2a. Got scene object\n" << std::flush;

            auto hittable = sceneObject->getHittable();

            std::cout << "2b. Got hittable\n" << std::flush;

            sceneWorld.add(hittable);

            std::cout << "2c. Added hittable\n" << std::flush;
        }

        std::cout << "3. Building BVH\n";

        auto bvh = std::make_shared<Geometry::bvh_node>(sceneWorld);

        std::cout << "4. BVH built successfully\n";

        sceneWorld = Geometry::hittable_list(bvh);

        std::cout << "5. Scene conversion finished\n";

        return sceneWorld;
    }

    void Aero_CPU_RT::fileWriterLoop(const std::vector<Renderer::tile>& tiles, const renderOptions& ctx, int startingTileIdx) {
        std::ofstream sidecard(ctx.fileName + ".arp", std::ios_base::app);
        std::ofstream framebufferDump(ctx.fileName + ".afd", std::ios_base::app);

        int totalTilesWritten = startingTileIdx;

        while (isFileWriterRunning || !finishedTileQueue.empty()) {
            int tileToWrite = -1;

            // Block until a tile is available or we are shutting down
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                queueCV.wait_for(lock, std::chrono::milliseconds(50), [this]() {
                    return !finishedTileQueue.empty() || !isFileWriterRunning;
                    });

                if (!finishedTileQueue.empty()) {
                    tileToWrite = finishedTileQueue.front();
                    finishedTileQueue.pop();
                }
            }

            // If we got a valid tile, dump it safely
            if (tileToWrite != -1 && tileToWrite < tiles.size()) {
                const auto& tile = tiles[tileToWrite];

                for (int j = tile.y_start; j < tile.y_end; j++) {
                    for (int i = tile.x_start; i < tile.x_end; i++) {
                        size_t idx = j * ctx.width + i;

                        framebufferDump << idx << " "
                            << ppmFramebuffer[idx].x() << " "
                            << ppmFramebuffer[idx].y() << " "
                            << ppmFramebuffer[idx].z() << "\n";
                    }
                }
                sidecard << tileToWrite << "\n";

                framebufferDump.flush();
                sidecard.flush();
                totalTilesWritten++;
            }

            if (totalTilesWritten >= tiles.size()) {
                break;
            }
        }
    }

    void Aero_CPU_RT::writeToPPM(const renderOptions& ctx) {
        std::ofstream ppmFile(ctx.fileName);
        if (!ppmFile) {
            std::clog << "Failed to open PPM output file: " << ctx.fileName << "\n";
            return;
        }

        ppmFile << "P3\n" << ctx.width << " " << ctx.height << "\n255\n";

        std::clog << "Finished rendering, writing image data to file..." << "\n";
        for (int y = 0; y < ctx.height; y++) {
            for (int x = 0; x < ctx.width; x++) {
                Aero::RT::Utils::write_color(ppmFile, ppmFramebuffer[y * ctx.width + x]);
            }
        }
    }

    // we are pulling corrupt data from teh AFD. futher insepction shows that there are missing sections filled with 0s. most likely a threading problem. test with the threading flag off. 
    // I need to implement gtest BADLY
    int Aero_CPU_RT::recoveryIndex(const renderOptions& ctx) {
        int maxTileIdxFound = -1; // Change to track the highest tile found
        std::ifstream renderProgressFile(ctx.fileName + ".arp");

        if (!renderProgressFile) {
            std::clog << "Failed to open ARP file: " << ctx.fileName + ".arp" << "\n";
            return -1;
        }

        if (renderProgressFile.is_open()) {
            int tileIndex;
            // Read integers directly, finding the true maximum
            while (renderProgressFile >> tileIndex) {
                if (tileIndex > maxTileIdxFound) {
                    maxTileIdxFound = tileIndex;
                }
            }
            renderProgressFile.close();
        }

        // If maxTileIdxFound is still -1, it means the file was empty (new render)
        int startingTileIdx = (maxTileIdxFound == -1) ? 0 : maxTileIdxFound + 1;

        // Check for completion
        if (startingTileIdx == (ctx.width * ctx.height) / (ctx.tile_size * ctx.tile_size)) {
            std::clog << "Render completed! No work to do.";
            return startingTileIdx;
        }

        // If resuming, pull historical float data from the afd dump back into memory
        if (startingTileIdx > 0) {
            std::clog << "\nResuming tile render starting from tile index: " << startingTileIdx << "\n";
            std::ifstream framebufferDumpRead(ctx.fileName + ".afd");
            if (!framebufferDumpRead) {
                std::clog << "Failed to open AFD file: " << ctx.fileName + ".afd" << "\n";
                return -1;
            }

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


    /*  void Aero_CPU_RT::renderNew(const Aero::RT::Camera::camera& cam,
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
    }*/
    }
