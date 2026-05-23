#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "util/common.hpp"

namespace Aero::RT::Materials {
	class dielectric : public material {
	public:
		dielectric(double refraction_index);

		bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override;

	private:
		// Refractive index in vacuum or air, or the ratio of the material's refractive index over
		// the refractive index of the enclosing media
		double refraction_index_;

		static double reflectance(double cosine, double refraction_index);
	};
}