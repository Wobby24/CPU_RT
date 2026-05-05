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
		sphere(const Math::point3& center, double radius, std::shared_ptr<Materials::material> mat) : center_(center), radius_(std::fmax(0, radius)), mat_(mat) {}

		bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override {
			Math::vec3 oc = center_ - r.origin();
			auto a = r.direction().length_squared();
			auto h = dot(r.direction(), oc);
			auto c = oc.length_squared() - radius_ * radius_;

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
				Math::vec3 outward_normal = (rec.p - center_) / radius_;
				rec.set_face_normal(r, outward_normal);
				rec.mat = mat_;

				return true;
		}
	private:
		Math::point3 center_;
		double radius_;
		std::shared_ptr<Materials::material> mat_;
	};
}
