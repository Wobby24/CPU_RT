cmake_minimum_required(VERSION 3.16)
project(CPURaytracer VERSION 1.0 LANGUAGES CXX C)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4  # Or "master" for the latest, but 3.4 is stable
    GIT_SHALLOW    TRUE
)

# Disable building GLFW documentation and tests to speed up your build
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)


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
)

# List Source files
set(PROJECT_SOURCES
    ${MATH_SOURCES}
    ${MATERIALS_SOURCES}
    ${GEOMETRY_SOURCES}
    ${CAMERA_SOURCES}
    #"${SRC_DIR}/CPU_Raytracer.cpp"
    "${SRC_DIR}/main.cpp"
)


# Post-build copy: only valid after the target exists
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_directory
  #  "${CMAKE_SOURCE_DIR}/rt_output"
   # "$<TARGET_FILE_DIR:${PROJECT_NAME}>/rt_output"
#)

add_library(OGL_RT_TextureView STATIC IMPORTED GLOBAL)

set_target_properties(OGL_RT_TextureView PROPERTIES
    IMPORTED_LOCATION
        "${LIBRARY_DIR}/OGL_RT_TextureView.lib"

    INTERFACE_INCLUDE_DIRECTORIES
        "${EXTERNAL_INCLUDE_DIR}"
    
    INTERFACE_LINK_LIBRARIES
        "glfw;OpenGL::GL"
)
# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE OGL_RT_TextureView)

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

# Debug macro
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
)
