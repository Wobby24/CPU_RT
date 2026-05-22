#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Materials {
	class lambertian : public material {
	public:
		lambertian(const Utils::color& albedo);

		bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override;
	private:
		Utils::color albedo_;
	};
}