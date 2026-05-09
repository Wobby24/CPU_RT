// simplify the camera class in RTIOW to something that is just data for wehre and how to render, not the renderer itself
#pragma once

#include "geometry/hittable.hpp"
#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace Aero::RT::Camera {
    class camera {
    public:

        camera() {};

        Math::ray get_ray(int i, int j) const {
            // Construct a camera ray originating from the defocus disk and directed at a randomly
            // sampled point around the pixel location i, j.

            auto offset = sample_squared();
            auto pixel_sample = pixel00_loc
                + ((i + offset.x()) * pixel_delta_u)
                + ((j + offset.y()) * pixel_delta_v);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return Math::ray(ray_origin, ray_direction);
        }

        int get_image_height() const { return image_height; }
        int get_image_width() const { return image_width; }
        int get_samples_per_pixel() const { return samples_per_pixel; }
        int get_max_depth() const { return max_depth; }
        double get_samples_per_pixel_scale() const { return pixel_samples_scale; }
        double get_fov() const { return vfov; }

        void set_image_width(int value) { image_width = value; }
        void set_aspect_ratio(double value) { aspect_ratio = value; }
        void set_samples_per_pixel(int value) { 
            samples_per_pixel = value;
            pixel_samples_scale = 1.0 / samples_per_pixel;
        }
        void set_max_depth(int value) { max_depth = value; }
        void set_fov(double value) { vfov = value; }

        void set_lookat(Math::point3 data) {
            lookat = data;
        }
        void set_lookfrom(Math::point3 data) {
            lookfrom = data;
        }
        void set_vup(Math::vec3<double> data) {
            vup = data;
        }

        void set_defocus_angle(double value) { defocus_angle = value; }
        void set_focus_distance(double value) { focus_dist = value; }

        //might not be needed quite yet if calculations for ray position are 
        //being done inside of this class 
        /*
        Math::vec3<double> get_pixel_delta_u() const { return pixel_delta_u; }
        Math::vec3<double> get_pixel_delta_v() const { return pixel_delta_v; }
        Math::point3 get_cam_center() const { return center; }
        Math::point3 get_pixel_origin_loc() const { return pixel00_loc; }
        */

        void initialize() {
            image_height = std::max(1, int(image_width / aspect_ratio));

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;

            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (double(image_width) / image_height);

            // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
            w = Math::unit_vector(lookfrom - lookat);
            u = Math::unit_vector(cross(vup, w));
            v = Math::cross(w, u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            Math::vec3<double> viewport_u = viewport_width * u; // Vector across viewport horizontal edge
            Math::vec3<double> viewport_v = viewport_height * -v; // Vector down viewport vertical edge

            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = center - (focus_dist* w) - viewport_u / 2 - viewport_v / 2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors.
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

    private:
        double aspect_ratio = 16.0 / 9.0;
        int image_width = 400; // default params
        int image_height;
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

        Math::point3 center; // Camera center
         
        Math::point3 pixel00_loc; // Location of pixel 0, 0
        Math::vec3<double> pixel_delta_u; // Offset to pixel to the right
        Math::vec3<double> pixel_delta_v; // Offset to pixel below
        Math::vec3<double> u, v, w; // Camera frame basis vectors
        Math::vec3<double>   defocus_disk_u;       // Defocus disk horizontal radius
        Math::vec3<double>   defocus_disk_v;       // Defocus disk vertical radius

        Math::vec3<double> sample_squared() const {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
            return Math::vec3<double>(random_value<double>() - 0.5, random_value<double>() - 0.5, 0);
        }

        Math::point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            auto p = Math::random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

    };
}