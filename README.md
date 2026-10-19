# Wobby's CPU_RT
A pathtracing engine (Raytracing is more well-known, that's why its RT, not PT) that is meant to be an improvement over my previous engine. It supports more features, cleaner code, better performance, and new ideas for faster development iteration.

![CPU Path Tracer](repo/render_pathtracer.gif)

## Goals

1. Triangle rendering
2. Model loading via tinyglTF
3. Fully texture driven materials
4. Multithreading via thread pools
5. SIMD acceleration via Google Highway
6. JSON configuartion, scene descriptor, and assest listing system
7. Vulkan RT scene previewer for rapid iteration
8. PNG, JPEG, and OpenEXR output

## Achievements
- Scene API for defining objects and cameras in a scene
- Triangle rendering supported (foundation for 3D models)

## WIP
- JSON scene description (scene constructor is currently being developed)
- glTF integration
- PBR materials
- SIMD
- Multithreading (current implementation is poorly written)

## Prerequisites
- System that can run DOOM? Yeah, this application is pretty easy to run (unless of course you want to render crazy high quality stuff). As long as you have a 64-bit CPU and more than 4GB of RAM, you should be fine
- If you want to use the live output tracking, you'll need a GPU capable of OpenGL 3.3 (but almost all iGPUs have support for it, same for discrete GPUs)
- Compiler that supports C++ 20
- CMake 3.16 or greater

## Build Instructions
This project uses CMake as its build system generator. All code should be tested to run on Windows and Linux (MacOS support is not a major priority right now, but will be in the future).
To build the project, run the following commands (work on both Linux and Windows):

    mkdir build
    cd build
    cmake ..
    cmake --build .

## Why a path tracing engine?
- Graphics work without the worry of GPU APIs and architecture
- Faster development
- Path tracing is a notoriously hard and resource-consuming task, and making it on the CPU first is a lot easier than developing on the GPU first

## Acknowledgements:

* [GLFW](https://www.glfw.org/)
* [stb_image](https://nothings.org/)
* [OpenGL](https://www.opengl.org/)
* [Raytracing in One Weekend Series](https://raytracing.github.io/)
* [GoogleTest](https://github.com/google/googletest)
* [nlohmann_json](https://github.com/nlohmann/json)
