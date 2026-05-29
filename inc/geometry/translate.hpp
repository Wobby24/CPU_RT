#pragma once

#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/aabb.hpp"
#include "math/interval.hpp"
#include "hittable.hpp"
#include <memory>

namespace Aero::RT::Geometry {
	class translate : public hittable {
	public:
		translate(std::shared_ptr<hittable> object, const Math::vec3<double>& offset);
		bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override;
		Math::aabb bounding_box() const override;
	private:
		std::shared_ptr<hittable> object_;
		Math::vec3<double> offset_;
		Math::aabb bbox_;
	};

	class rotate_y : public hittable {
	public:
		rotate_y(std::shared_ptr<hittable> object, double angle);
		bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override;
		Math::aabb bounding_box() const override;
	private:
		std::shared_ptr<hittable> object_;
		double sin_theta_;
		double cos_theta_;
		Math::aabb bbox_;
	};
}