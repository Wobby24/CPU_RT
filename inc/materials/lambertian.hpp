#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Materials {
	class lambertian : public material {
	public:
		lambertian(const Utils::color& albedo) : albedo_(albedo) {}

		bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override {
			auto scatter_direction = rec.normal + Math::random_unit_vector();

			// Catch degenerate scatter direction
			if (scatter_direction.near_zero())
				scatter_direction = rec.normal;

			scattered = Math::ray(rec.p, scatter_direction);
			attenuation = albedo_;
			return true;
		}
	private:
		Utils::color albedo_;
	};
}