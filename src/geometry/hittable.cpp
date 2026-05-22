#include "geometry/hittable.hpp"

namespace Aero::RT::Geometry {

	hit_record::hit_record() : front_face(false), t(0) {}

	void hit_record::set_face_normal(const Math::ray& r, const Math::vec3<double>& outward_normal) {
		// Sets the hit record normal vector.
		// Note: the parameter `outward_normal` is assumed to have unit length

		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
}