#include "materials/metal.hpp"
#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Materials {
	metal::metal(const Utils::color& albedo, double fuzz) : albedo_(albedo), fuzz_(fuzz < 1 ? fuzz : 1) {}

	bool metal::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
		Math::vec3<double> reflected = Math::reflect(r_in.direction(), rec.normal);
		reflected = Math::unit_vector(reflected) + (fuzz_ * Math::random_unit_vector());
		scattered = Math::ray(rec.p, reflected, r_in.time());
		attenuation = albedo_;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
}