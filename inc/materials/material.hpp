#pragma once

#include "geometry/hittable.hpp"
#include "math/ray.hpp"
#include "util/color.hpp"

namespace Aero::RT::Materials {
	class material {
	public:
		virtual ~material() = default;

		// default imp to ensure we can use pointer to generic material type
		virtual bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const = 0;

		virtual Utils::color emitted(double u, double v, const Math::point3& p) const {
			return Utils::color(0, 0, 0);
		}
	};
}