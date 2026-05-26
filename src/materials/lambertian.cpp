#include "materials/lambertian.hpp"
#include "textures/solid_color.hpp"
#include <memory>

namespace Aero::RT::Materials {
    lambertian::lambertian(const Utils::color& albedo) : tex_(std::make_shared<Textures::solid_color>(albedo)) {}

	lambertian::lambertian(std::shared_ptr<Textures::texture> tex) : tex_(tex) {}

    bool lambertian::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
			auto scatter_direction = rec.normal + Math::random_unit_vector();

			// Catch degenerate scatter direction
			if (scatter_direction.near_zero())
				scatter_direction = rec.normal;

			scattered = Math::ray(rec.p, scatter_direction, r_in.time());
			attenuation = tex_->value(rec.u, rec.v, rec.p);
			return true;
	}
}