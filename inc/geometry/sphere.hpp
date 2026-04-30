#pragma once

#include "hittable.hpp"
#include "vec3.hpp"
#include <cmath>

namespace Aero::RT::Geometry::Hittables {
	class sphere : public Aero::RT::Math::hittable {
	public:
		sphere(const Aero::RT::Math::point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

		bool hit(const Aero::RT::Math::ray& r, double ray_tmin, double ray_tmax, Aero::RT::Math::hit_record& rec) const override {
			Aero::RT::Math::vec3 oc = center - r.origin();
			auto a = r.direction().length_squared();
			auto h = dot(r.direction(), oc);
			auto c = oc.length_squared() - radius * radius;

			auto discriminant = h * h - a * c;
			if (discriminant < 0) 
				return false;

				auto sqrtd = std::sqrt(discriminant);

				// Find the nearest root that lies in teh acceptable range.
				auto root = (h - sqrtd) / a;
				if (root <= ray_tmin || ray_tmax <= root) {
					root = (h + sqrtd) / a;
					if (root <= ray_tmin || ray_tmax < +root)
						return false;
				}
			
				rec.t = root;
				rec.p = r.at(rec.t);
				rec.normal = (rec.p - center) / radius;

				return true;
		}
	private:
		Aero::RT::Math::point3 center;
		double radius;
	};
}
