#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"
#include "geometry/hittable.hpp"

namespace Aero::RT {
	Utils::color compute_ray_color(const Math::ray& r, const Geometry::hittable& world) {
		Geometry::hit_record rec;
		if (world.hit(r, Math::interval(0, infinity), rec)) {
			return 0.5 * (rec.normal + Utils::color(1, 1, 1));
		}

		Math::vec3<double> unit_direction = Math::unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * Utils::color(1.0, 1.0, 1.0) + a * Utils::color(0.5, 0.7, 1.0);
	}
}