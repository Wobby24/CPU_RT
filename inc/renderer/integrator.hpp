#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"

namespace Aero::RT::Renderer {
	Utils::color ray_color(const Math::ray& r, int depth, const Geometry::hittable& world) {
		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
			return Utils::color(0, 0, 0);

		Geometry::hit_record rec;
		// 0.001 for bias to correct shadow acne
		if (world.hit(r, Math::interval(0.001, infinity), rec)) {
			Math::ray scattered;
			Utils::color attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered)) {
				// recursion
				return attenuation * ray_color(scattered, depth - 1, world);
			}
			
			return Utils::color(0, 0, 0);
		}

		Math::vec3<double> unit_direction = Math::unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * Utils::color(1.0, 1.0, 1.0) + a * Utils::color(0.5, 0.7, 1.0);
	}
}