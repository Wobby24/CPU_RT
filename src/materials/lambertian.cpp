#include "materials/lambertian.hpp"

namespace Aero::RT::Materials {
    lambertian::lambertian(const Utils::color& albedo) : albedo_(albedo) {}

    bool lambertian::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
			auto scatter_direction = rec.normal + Math::random_unit_vector();

			// Catch degenerate scatter direction
			if (scatter_direction.near_zero())
				scatter_direction = rec.normal;

			scattered = Math::ray(rec.p, scatter_direction, r_in.time());
			attenuation = albedo_;
			return true;
	}
}