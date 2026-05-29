#include "geometry/translate.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/aabb.hpp"
#include "math/interval.hpp"
#include "geometry/hittable.hpp"
#include "util/common.hpp"
#include <memory>
#include <cmath>

namespace Aero::RT::Geometry {

	// Translate //

	translate::translate(std::shared_ptr<hittable> object, const Math::vec3<double>& offset) 
		: object_(object), offset_(offset)
	{
		bbox_ = object_->bounding_box() + offset_;
	}

	bool translate::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
		// Move the ray backwards by the offset ]
		Math::ray offset_r(r.origin() - offset_, r.direction(), r.time());

		// Determine whether an intersection exists along the offset ray (and if so, where)
		if (!object_->hit(offset_r, ray_t, rec))
			return false;

		// Move the intersection point forwards by the offset
		rec.p += offset_;

		return true;
	}

	Math::aabb translate::bounding_box() const {
		return bbox_;
	}

	// Rotate_y // 

	rotate_y::rotate_y(std::shared_ptr<hittable> object, double angle) : object_(object) {
		auto radians = degrees_to_radians(angle);
		sin_theta_ = std::sin(radians);
		cos_theta_ = std::cos(radians);
		bbox_ = object_->bounding_box();

		Math::point3 min(infinity, infinity, infinity);
		Math::point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox_.x.max + (1 - i) * bbox_.x.min;
					auto y = j * bbox_.y.max + (1 - j) * bbox_.y.min;
					auto z = k * bbox_.z.max + (1 - k) * bbox_.z.min;

					auto newx = cos_theta_ * x + sin_theta_ * z;
					auto newz = -sin_theta_ * x + cos_theta_ * z;

					Math::vec3<double> tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox_ = Math::aabb(min, max);
	}

	bool rotate_y::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
		// Transform the ray from world space to object space

		auto origin = Math::point3((cos_theta_ * r.origin().x()) - (sin_theta_ * r.origin().z()),
			r.origin().y(),
			(sin_theta_ * r.origin().x()) + (cos_theta_ * r.origin().z())
		);

		auto direction = Math::vec3<double>((cos_theta_ * r.direction().x()) - (sin_theta_ * r.direction().z()),
			r.direction().y(),
			(sin_theta_ * r.direction().x()) + (cos_theta_ * r.direction().z())
		);

		Math::ray rotated_r(origin, direction, r.time());

		// Determine whether an intersection exists in object space (and if so, where);

		if (!object_->hit(rotated_r, ray_t, rec))
			return false;

		// Transform the intersection from object space back to world space

		rec.p = Math::point3((cos_theta_ * rec.p.x()) + (sin_theta_ * rec.p.z()),
			rec.p.y(),
			(-sin_theta_ * rec.p.x()) + (cos_theta_ * rec.p.z())
		);

		rec.normal = Math::vec3<double>((cos_theta_ * rec.normal.x()) + (sin_theta_ * rec.normal.z()),
			rec.normal.y(),
			(-sin_theta_ * rec.normal.x()) + (cos_theta_ * rec.normal.z())
		);

		return true;
	}

	Math::aabb rotate_y::bounding_box() const {
		return bbox_;
	}
 }

