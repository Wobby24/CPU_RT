#pragma once

#include "hittable.hpp"
#include "math/vec3.hpp"
#include <cmath>

namespace Aero::RT::Geometry {
	class sphere : public hittable {
	public:
		sphere(const Math::point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

		bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override {
			Math::vec3 oc = center - r.origin();
			auto a = r.direction().length_squared();
			auto h = dot(r.direction(), oc);
			auto c = oc.length_squared() - radius * radius;

			auto discriminant = h * h - a * c;
			if (discriminant < 0) 
				return false;

				auto sqrt_d = std::sqrt(discriminant);

				// Find the nearest root that lies in teh acceptable range.
				auto root = (h - sqrt_d) / a;
				if (!ray_t.surrounds(root)) {
					root = (h + sqrt_d) / a;
					if (!ray_t.surrounds(root))
						return false;
				}
			
				rec.t = root;
				rec.p = r.at(rec.t);
				Math::vec3 outward_normal = (rec.p - center) / radius;
				rec.set_face_normal(r, outward_normal);

				return true;
		}
	private:
		Math::point3 center;
		double radius;
	};
}
