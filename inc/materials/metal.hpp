#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Materials {
	class metal : public material {
	public:
		metal(const Utils::color& albedo, double fuzz) : albedo_(albedo), fuzz_(fuzz < 1 ? fuzz : 1) {}

		bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override {
			Math::vec3<double> reflected = Math::reflect(r_in.direction(), rec.normal);
			reflected = Math::unit_vector(reflected) + (fuzz_ * Math::random_unit_vector());
			scattered = Math::ray(rec.p, reflected);
			attenuation = albedo_;
			return (dot(scattered.direction(), rec.normal) > 0);
		}

	private:
		Utils::color albedo_;
		double fuzz_;
	};
}