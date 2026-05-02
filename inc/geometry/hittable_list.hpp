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

		hittable_list() {}
		hittable_list(std::shared_ptr<hittable> object) { add(object); }

		void clear() { objects.clear(); }

		void add(std::shared_ptr<hittable> object) {
			objects.push_back(object);
		}

		bool hit(const Math::ray& r, Math::interval ray_t, Geometry::hit_record& rec) const override {
			hit_record temp_rec;
			bool hit_anything = false;
			auto closest_so_far = ray_t.max;

			for (const auto& object : objects) {
				if (object->hit(r, Math::interval(ray_t.min, closest_so_far), temp_rec)) {
					hit_anything = true;
					closest_so_far = temp_rec.t;
					rec = temp_rec;
				}
			}

			return hit_anything;
		}
	};
}