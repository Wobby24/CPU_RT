#pragma once

#include "hittable.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"

#include <memory>
#include <vector>

namespace Aero::RT::Geometry {
	class hittable_list : public hittable {
	public:
		std::vector<std::shared_ptr<hittable>> objects;

		hittable_list();
		hittable_list(std::shared_ptr<hittable> object);

		void clear();

		void add(std::shared_ptr<hittable> object);

		bool hit(const Math::ray& r, Math::interval ray_t, Geometry::hit_record& rec) const override;
	};
}