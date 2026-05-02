// simplify the camera class in RTIOW to something that is just data for wehre and how to render, not the renderer itself
#pragma once

#include "geometry/hittable.hpp"
#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace Aero::RT::Camera {
    class camera {
    public:

        camera() {
            initialize();
        }

        Math::ray get_ray(int i, int j) const {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - center;
            return Math::ray(center, ray_direction);
        }

        int get_image_height() const { return image_height; }
        int get_image_width() const { return image_width; }
        int set_image_width(int value) { image_width = value; }
        int set_aspect_ratio(double value) { aspect_ratio = value; }

        //might not be needed quite yet if calculations for ray position are 
        //being done inside of this class 
        /*
        Math::vec3<double> get_pixel_delta_u() const { return pixel_delta_u; }
        Math::vec3<double> get_pixel_delta_v() const { return pixel_delta_v; }
        Math::point3 get_cam_center() const { return center; }
        Math::point3 get_pixel_origin_loc() const { return pixel00_loc; }
        */

    private:
        double aspect_ratio = 16.0 / 9.0;
        int image_width = 400; // default params
        int image_height;

        Math::point3 center;

        Math::point3 pixel00_loc;
        Math::vec3<double> pixel_delta_u;
        Math::vec3<double> pixel_delta_v;

        void initialize() {
            image_height = std::max(1, int(image_width / aspect_ratio));

            //int image_height = int(image_width / aspect_ratio);
            //image_height = (image_height < 1) ? 1 : image_height;

            center = Math::point3(0, 0, 0);

            double focal_length = 1.0;
            double viewport_height = 2.0;
            double viewport_width = viewport_height * (double(image_width) / image_height);

            Math::vec3<double> viewport_u(viewport_width, 0, 0);
            Math::vec3<double> viewport_v(0, -viewport_height, 0);

            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            auto viewport_upper_left =
                center - Math::vec3<double>(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }
    };
}