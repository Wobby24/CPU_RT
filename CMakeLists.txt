cmake_minimum_required(VERSION 3.16)
project(CPURaytracer VERSION 1.0 LANGUAGES CXX C)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent
include(FetchContent)

FetchContent_Declare(
    OGL_RT_TextureView
    GIT_REPOSITORY https://github.com/Wobby24/OGL_RT_TextureView.git
    GIT_TAG        v1.0.2
)

FetchContent_MakeAvailable(OGL_RT_TextureView)

# Default to Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Output binary location
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project directories (based on current repo layout)
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")  # keep forward slashes and keep on one line
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/inc")
set(EXTERNAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ext")
set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib")

set(CAMERA_SOURCES
    "${SRC_DIR}/camera/camera.cpp"
) 

set(GEOMETRY_SOURCES
    "${SRC_DIR}/geometry/hittable.cpp"
    "${SRC_DIR}/geometry/hittable_list.cpp"
    "${SRC_DIR}/geometry/sphere.cpp"
    "${SRC_DIR}/geometry/bvh.cpp"
    "${SRC_DIR}/geometry/quad.cpp"
    "${SRC_DIR}/geometry/triangle.cpp"
)

set(MATERIALS_SOURCES
    "${SRC_DIR}/materials/dielectric.cpp"
    "${SRC_DIR}/materials/lambertian.cpp"
    "${SRC_DIR}/materials/metal.cpp"
)

set(MATH_SOURCES
    "${SRC_DIR}/math/interval.cpp"
    "${SRC_DIR}/math/ray.cpp"
    "${SRC_DIR}/math/aabb.cpp"
)

set(UTIL_SOURCES 
    "${SRC_DIR}/util/color.cpp"
    "${SRC_DIR}/util/image_loader.cpp"
    "${SRC_DIR}/util/perlin_noise.cpp"
)

set(TEXTURE_SOURCES
    "${SRC_DIR}/textures/solid_color.cpp"
    "${SRC_DIR}/textures/checker_texture.cpp"
    "${SRC_DIR}/textures/image_texture.cpp"
    "${SRC_DIR}/textures/noise_texture.cpp"
)

# List Source files
set(PROJECT_SOURCES
    ${MATH_SOURCES}
    ${MATERIALS_SOURCES}
    ${GEOMETRY_SOURCES}
    ${CAMERA_SOURCES}
    ${UTIL_SOURCES}
    ${TEXTURE_SOURCES}
    #"${SRC_DIR}/CPU_Raytracer.cpp"
    "${SRC_DIR}/main.cpp"
)

# Post-build copy: only valid after the target exists
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_directory
  #  "${CMAKE_SOURCE_DIR}/rt_output"
   # "$<TARGET_FILE_DIR:${PROJECT_NAME}>/rt_output"
#)

# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Post-build copy: Copies the 'res' folder into the same directory as the executable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)

target_link_libraries(${PROJECT_NAME} PRIVATE  OGL_RT_TextureView::OGL_RT_TextureView)

# Includes
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${INCLUDE_DIR}"
        "${EXTERNAL_INCLUDE_DIR}"
)

file(GLOB_RECURSE PROJECT_HEADERS
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/*.hpp"
    #maybe add ext?
)

# Debug macro
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
)
