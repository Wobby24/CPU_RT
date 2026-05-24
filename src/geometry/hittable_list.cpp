#include "geometry/hittable_list.hpp"
#include "geometry/hittable.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"

#include <memory>
#include <vector>

namespace Aero::RT::Geometry {

	hittable_list::hittable_list() {}

	hittable_list::hittable_list(std::shared_ptr<hittable> object) { 
		add(object);
	}

	void hittable_list::clear() { 
		objects.clear(); 
	}

	void hittable_list::add(std::shared_ptr<hittable> object) {
		objects.push_back(object);
	}

	bool hittable_list::hit(const Math::ray& r, Math::interval ray_t, Geometry::hit_record& rec) const {
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
}