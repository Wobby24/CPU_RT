#pragma once

#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace Aero::RT::Camera {
    class camera {
    public:
        camera();

        Math::ray get_ray(int i, int j) const;

        double get_fov() const { return vfov; }
        double get_defocus_angle() const { return defocus_angle; }
        double get_focus_distance() const { return focus_dist; }

        void set_fov(double value) { vfov = value; }
        void set_lookfrom(Math::point3 data) { lookfrom = data; }
        void set_lookat(Math::point3 data) { lookat = data; }
        void set_vup(Math::vec3<double> data) { vup = data; }
        void set_defocus_angle(double value) { defocus_angle = value; }
        void set_focus_distance(double value) { focus_dist = value; }

        void initialize(int width, int height);

    private:
        double vfov = 90.0;               // Vertical view angle (field of view)
        double defocus_angle = 0.0;       // Variation angle of rays through each pixel
        double focus_dist = 10.0;         // Distance from lookfrom to the plane of perfect focus

        Math::point3 lookfrom = Math::point3(0, 0, 0);   // Camera position
        Math::point3 lookat = Math::point3(0, 0, -1);    // Point camera faces
        Math::vec3<double> vup = Math::vec3<double>(0, 1, 0); // Camera "up" direction

        Math::point3 center = Math::point3(0, 0, 0);
        Math::point3 pixel00_loc;         // Location of pixel (0, 0)
        Math::vec3<double> pixel_delta_u; // Offset vector to pixel right
        Math::vec3<double> pixel_delta_v; // Offset vector to pixel below
        Math::vec3<double> u, v, w;       // Camera coordinate system basis vectors
        Math::vec3<double> defocus_disk_u; // Defocus disk horizontal radius
        Math::vec3<double> defocus_disk_v; // Defocus disk vertical radius

        // Internal helpers for sub-pixel ray generation
        Math::vec3<double> sample_squared() const;
        Math::point3 defocus_disk_sample() const;
    };
}