#include "camera/camera.hpp"
#include "geometry/hittable.hpp"
#include "math/vec3.hpp"
#include "math/ray.hpp"

namespace Aero::RT::Camera {
    camera::camera() {}

	Math::ray camera::get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
          // sampled point around the pixel location i, j.

        auto offset = sample_squared();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_value<double>();

        return Math::ray(ray_origin, ray_direction, ray_time);
	}

    void camera::initialize(int width, int height) {
        double aspect_ratio = static_cast<double>(width) / height;

        center = lookfrom;

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * aspect_ratio;

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = Math::unit_vector(lookfrom - lookat);
        u = Math::unit_vector(cross(vup, w));
        v = Math::cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Math::vec3<double> viewport_u = viewport_width * u; // Vector across viewport horizontal edge
        Math::vec3<double> viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        pixel_delta_u = viewport_u / width;
        pixel_delta_v = viewport_v / height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Math::vec3<double> camera::sample_squared() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Math::vec3<double>(random_value<double>() - 0.5, random_value<double>() - 0.5, 0);
    }

    Math::point3 camera::defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = Math::random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
}