#pragma once

#include "math/ray.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Geometry {
	class hit_record {
	public:
		Aero::RT::Math::point3 p;
		Aero::RT::Math::vec3 normal;
		double t;
	};

	class hittable { // hittable objects inherit from this
	public:
		virtual ~hittable() = default;

		virtual bool hit(const Aero::RT::Math::ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
	};
}