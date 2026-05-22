#pragma once

#include "hittable.hpp"
#include "math/vec3.hpp"
#include "materials/material.hpp"
#include <cmath>
#include <memory>

namespace Aero::RT::Geometry {
	class sphere : public hittable {
	public:
		// avoid using specific namespace with material to ensure forward declaration doesn't break
		sphere(const Math::point3& center, double radius, std::shared_ptr<Materials::material> mat);

		bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override;
	private:
		Math::point3 center_;
		double radius_;
		std::shared_ptr<Materials::material> mat_;
	};
}
