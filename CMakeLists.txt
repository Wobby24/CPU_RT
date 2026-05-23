cmake_minimum_required(VERSION 3.16)
project(CPURaytracer VERSION 1.0 LANGUAGES CXX C)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(CAMERA_SOURCES
    "${SRC_DIR}/camera/camera.cpp"
) 

set(GEOMETRY_SOURCES
    "${SRC_DIR}/geometry/hittable.cpp"
    "${SRC_DIR}/geometry/hittable_list.cpp"
    "${SRC_DIR}/geometry/sphere.cpp"
)

set(MATERIALS_SOURCES
    "${SRC_DIR}/materials/dielectric.cpp"
    "${SRC_DIR}/materials/lambertian.cpp"
    "${SRC_DIR}/materials/metal.cpp"
)

set(MATH_SOURCES
    "${SRC_DIR}/math/interval.cpp"
    "${SRC_DIR}/math/ray.cpp"
    "${SRC_DIR}/math/vec3.cpp"
)

set(RENDERER_SOURCES
    "${SRC_DIR}/renderer/integrator.cpp"
)

# List Source files
set(PROJECT_SOURCES
    ${RENDERER_SOURCES}
    ${MATH_SOURCES}
    ${MATERIALS_SOURCES}
    ${GEOMETRY_SOURCES}
    ${CAMERA_SOURCES}
    "${SRC_DIR}/CPU_Raytracer.cpp"
    "${SRC_DIR}/main.cpp"
)

# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Post-build copy: only valid after the target exists
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_directory
  #  "${CMAKE_SOURCE_DIR}/rt_output"
   # "$<TARGET_FILE_DIR:${PROJECT_NAME}>/rt_output"
#)

# Includes
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${INCLUDE_DIR}"
        "${EXTERNAL_INCLUDE_DIR}"
)

file(GLOB_RECURSE PROJECT_HEADERS
    "${INCLUDE_DIR}/*.h"
    "${INCLUDE_DIR}/*.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# Debug macro
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
)
