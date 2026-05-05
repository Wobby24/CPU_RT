#pragma once

#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/interval.hpp"
#include <memory>

namespace Aero::RT::Materials {
	class material; // forward declaration
}


namespace Aero::RT::Geometry {
	class hit_record {
	public:
		Math::point3 p;
		Math::vec3<double> normal;
		std::shared_ptr<Materials::material> mat;
		double t;
		bool front_face;

		void set_face_normal(const Math::ray& r, const Math::vec3<double>& outward_normal) {
			// Sets the hit record normal vector.
			// Note: the parameter `outward_normal` is assumed to have unit length

			front_face = dot(r.direction(), outward_normal) < 0;
			normal = front_face ? outward_normal : -outward_normal;
		}
	};

	class hittable { // hittable objects inherit from this
	public:
		virtual ~hittable() = default;

		virtual bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const = 0;
	};
}