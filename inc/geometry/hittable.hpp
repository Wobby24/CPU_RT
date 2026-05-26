#pragma once

#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/aabb.hpp"
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
		double u;
		double v;
		bool front_face;

		void set_face_normal(const Math::ray& r, const Math::vec3<double>& outward_normal);
	};

	class hittable { // hittable objects inherit from this
	public:
		virtual ~hittable() = default;

		virtual bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const = 0;

		virtual Math::aabb bounding_box() const = 0;
	};
}