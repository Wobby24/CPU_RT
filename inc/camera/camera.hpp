#pragma once

#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace Aero::RT::Camera {
    class camera {
    public:

        camera();

        Math::ray get_ray(int i, int j) const;

        // get-methods
        int get_image_height() const { return image_height; }
        int get_image_width() const { return image_width; }
        int get_samples_per_pixel() const { return samples_per_pixel; }
        int get_max_depth() const { return max_depth; }
        double get_samples_per_pixel_scale() const { return pixel_samples_scale; }
        double get_fov() const { return vfov; }

        // set-methods
        void set_image_width(int value) { image_width = value; }
        void set_aspect_ratio(double value) { aspect_ratio = value; }
        void set_samples_per_pixel(int value);
        void set_max_depth(int value) { max_depth = value; }
        void set_fov(double value) { vfov = value; }

        void set_lookat(Math::point3 data) { lookat = data; }
        void set_lookfrom(Math::point3 data) { lookfrom = data; }
        void set_vup(Math::vec3<double> data) { vup = data; }

        void set_defocus_angle(double value) { defocus_angle = value; }
        void set_focus_distance(double value) { focus_dist = value; }

        void initialize();

    private:
        double aspect_ratio = 16.0 / 9.0;
        int image_width = 400; // default params
        int image_height = 0;
        int samples_per_pixel = 10; // count of random samples for each pixel
        int max_depth = 10; // maximum number of ray bounces into scene
        double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
        double vfov = 90; // Vertical view angle (field of view)

        double defocus_angle = 0;  // Variation angle of rays through each pixel
        double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

        // position and direction stuff 
        Math::point3 lookfrom = Math::point3(0, 0, 0); // Point camera is looking from
        Math::point3 lookat = Math::point3(0, 0, -1); // Point camera is looking at
        Math::vec3<double> vup = Math::vec3<double>(0, 1, 0); // Camera-relative "up" direction

        Math::point3 center = Math::point3(0, 0, 0); // Camera center
         
        Math::point3 pixel00_loc; // Location of pixel 0, 0
        Math::vec3<double> pixel_delta_u; // Offset to pixel to the right
        Math::vec3<double> pixel_delta_v; // Offset to pixel below
        Math::vec3<double> u, v, w; // Camera frame basis vectors
        Math::vec3<double>   defocus_disk_u;       // Defocus disk horizontal radius
        Math::vec3<double>   defocus_disk_v;       // Defocus disk vertical radius

        Math::vec3<double> sample_squared() const;

        Math::point3 defocus_disk_sample() const;
    };
}