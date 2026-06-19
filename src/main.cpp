#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <sstream>

#include "demo_scenes/bouncing_spheres.hpp"
#include "demo_scenes/checkered_spheres.hpp"
#include "demo_scenes/earth.hpp"
#include "demo_scenes/perlin_spheres.hpp"
#include "demo_scenes/quads.hpp"
#include "demo_scenes/simple_light.hpp"
#include "demo_scenes/cornell_box.hpp"
#include "demo_scenes/cornell_smoke_boxes.hpp"
#include "demo_scenes/book_2_final_scene.hpp"

#include "OGL_RT_TextureView.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "CPU_Raytracer.hpp"
#include "renderer/renderOptions.hpp"
#include "renderer/tile.hpp"
#include "simd/vec4_simd.hpp"
#include "math/vec4.hpp"
#include <chrono>

void testSIMDAlignment() {
    constexpr size_t ITERATIONS = 10'000'000; // 10 million iterations
    std::cout << "Running benchmark with " << ITERATIONS << " iterations...\n\n";

    // -------------------------------------------------------------
    // 1. BENCHMARK STANDARD CLASS
    // -------------------------------------------------------------
    {
        using Aero::RT::Math::vec4;
        vec4<double> total(0.0, 0.0, 0.0, 0.0);
        vec4<double> step(1.0, 1.1, 1.2, 1.3);
        double total_length = 0.0;

        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < ITERATIONS; ++i) {
            total += step;
            total *= 1.00001;
            total_length += total.length_squared();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "--- Standard vec4<double> Result ---\n";
        std::cout << "Time taken: " << duration.count() << " ms\n";
        std::cout << "Dummy output (prevents optimization): " << total.x() + total_length << "\n\n";
    }

    // -------------------------------------------------------------
    // 2. BENCHMARK SIMD CLASS
    // -------------------------------------------------------------
    {
        using Aero::RT::Simd::vec4_simd;
        vec4_simd total(0.0, 0.0, 0.0, 0.0);
        vec4_simd step(1.0, 1.1, 1.2, 1.3);
        double total_length = 0.0;

        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < ITERATIONS; ++i) {
            total += step;
            total *= 1.00001;
            total_length += total.length_squared();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "--- SIMD vec4_simd Result ---\n";
        std::cout << "Time taken: " << duration.count() << " ms\n";
        std::cout << "Dummy output (prevents optimization): " << total.x() + total_length << "\n\n";
    }
}

int main() {
   
    // get our data setup
    Aero::RT::Aero_CPU_RT renderer;
    Aero::RT::renderOptions options;
    Aero::RT::Geometry::hittable_list world;
    Aero::RT::Camera::camera cam;
    Aero::GL::OGL_RT_TextureViewer texViewer;

    options.multithreaded = true;
    options.threadCount = 8;

    // dont do this, make a scene class for the love of all things good. the scene class can the pass its hittable list to the renderer and we shuld be good

    switch (7) {
        case 1: Aero::RT::Demo_Scenes::bouncing_spheres(world, cam, options);  break;
        case 2: Aero::RT::Demo_Scenes::checkered_spheres(world, cam, options); break;
        case 3: Aero::RT::Demo_Scenes::earth(world, cam, options); break;
        case 4: Aero::RT::Demo_Scenes::perlin_spheres(world, cam, options); break;
        case 5: Aero::RT::Demo_Scenes::quads(world, cam, options); break;
        case 6: Aero::RT::Demo_Scenes::simple_lights(world, cam, options); break;
        case 7: Aero::RT::Demo_Scenes::cornell_box(world, cam, options); break;
        case 8: Aero::RT::Demo_Scenes::cornell_smoke_boxes(world, cam, options); break;
        case 9: Aero::RT::Demo_Scenes::book_2_final_scene(world, cam, options, true); break; // HQ render
        case 10: Aero::RT::Demo_Scenes::book_2_final_scene(world, cam, options, false); break; // LQ render
    }

   // cam.initialize(options.width, options.height);
    testSIMDAlignment();

  //  std::vector<Aero::RT::Renderer::tile> tiles = Aero::RT::Renderer::dispatchTiles(options.width, options.height, options.tile_size);

    // eventually we wont need this and can just move it onto threadpool
  //  std::thread tracerThread([&, tiles]() {
     //   renderer.render(cam, world, options);
   //  });

    // pre-allocate our framebuffer that the output viewer displays
  //  std::vector<uint32_t> bufferCopy(options.width * options.height);

    // Interactive responsive execution window loop
 //   while (!texViewer.shouldWindowClose()) {
    //    renderer.copy_framebuffer(bufferCopy);

     //   texViewer.renderToTexture(bufferCopy, options.width, options.height);

        // Frame limiter rest slice to avoid maxing out a full primary thread pipeline state
      //  std::this_thread::sleep_for(std::chrono::milliseconds(16));
  //  }
  //  renderer.stop_rendering();
  // if (tracerThread.joinable()) {
  //      tracerThread.join();
  //  }

    return 0;
}