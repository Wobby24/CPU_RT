#include "materials/dielectric.hpp"
#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"
#include "util/common.hpp"

namespace Aero::RT::Materials {
    dielectric::dielectric(double refraction_index) : refraction_index_(refraction_index) {}

    bool dielectric::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
			attenuation = Utils::color(1.0, 1.0, 1.0);
			double ri = rec.front_face ? (1.0 / refraction_index_) : refraction_index_;

			Math::vec3<double> unit_direction = Math::unit_vector(r_in.direction());
			double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
			double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

			bool cannot_refract = ri * sin_theta > 1.0;
			Math::vec3<double> direction;

			if (cannot_refract || reflectance(cos_theta, ri) > random_value<double>()) {
				direction = Math::reflect(unit_direction, rec.normal);
			}
			else {
				direction = Math::refract(unit_direction, rec.normal, ri);
			}


			scattered = Math::ray(rec.p, direction);
			return true;
    }

    static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
}

