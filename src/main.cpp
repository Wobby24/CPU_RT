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
#include "hwy/highway.h"
#include "CPU_Raytracer.hpp"
#include "renderer/renderOptions.hpp"
#include "renderer/tile.hpp"

namespace hn = hwy::HWY_NAMESPACE;


// functoin reports using SSE2, but our CPU is capable of AVX2 (4 floats vs 8 floats) so we need to dynamically dispatch. It's most likely it falls back to a stable SIMD instruction set
void testSIMDAlignment() {
    // 1. Define our hardware vector descriptor for 32-bit floats
    constexpr hn::ScalableTag<float> d;
    size_t lanes = hn::Lanes(d);

    std::cout << "[SIMD Test] Your CPU is processing " << lanes << " floats per register!\n";

    // 2. Create some basic mock data matching our lane size
    std::vector<float> arrayA(lanes, 10.0f); // Fill with 10.0
    std::vector<float> arrayB(lanes, 5.5f);  // Fill with 5.5
    std::vector<float> arrayC(lanes, 0.0f);  // Storage for results

    // 3. THE SIMD OPERATION:
    // Load data from normal RAM into SIMD hardware registers
    auto vecA = hn::Load(d, arrayA.data());
    auto vecB = hn::Load(d, arrayB.data());

    // Execute ONE instruction to add all lanes simultaneously!
    auto vecResult = hn::Add(vecA, vecB);

    // Store the hardware register data back into standard RAM
    hn::Store(vecResult, d, arrayC.data());

    // 4. Verify the silicon did the math right
    std::cout << "[SIMD Test] Result at index 0: " << arrayC[0] << " (Expected: 15.5)\n";
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

    cam.initialize(options.width, options.height);
    testSIMDAlignment();

    std::vector<Aero::RT::Renderer::tile> tiles = Aero::RT::Renderer::dispatchTiles(options.width, options.height, options.tile_size);

    // eventually we wont need this and can just move it onto threadpool
    std::thread tracerThread([&, tiles]() {
        renderer.render(cam, world, options);
     });

    // pre-allocate our framebuffer that the output viewer displays
    std::vector<uint32_t> bufferCopy(options.width * options.height);

    // Interactive responsive execution window loop
    while (!texViewer.shouldWindowClose()) {
        renderer.copy_framebuffer(bufferCopy);

        texViewer.renderToTexture(bufferCopy, options.width, options.height);

        // Frame limiter rest slice to avoid maxing out a full primary thread pipeline state
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    renderer.stop_rendering();
   if (tracerThread.joinable()) {
        tracerThread.join();
    }

    return 0;
}