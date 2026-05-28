#include "geometry/quad.hpp"
#include "geometry/hittable.hpp"
#include "math/vec3.hpp"
#include "math/ray.hpp"
#include "math/aabb.hpp"
#include "math/interval.hpp"
#include "materials/material.hpp"

namespace Aero::RT::Geometry {
	quad::quad(const Math::point3& Q, const Math::vec3<double>& u, const Math::vec3<double>& v, std::shared_ptr<Materials::material> mat)
		: Q_(Q), u_(u), v_(v), mat_(mat)
	{
		set_bounding_box();
	}

	void quad::set_bounding_box() {
		// CCompute the bounding box of all four vertices
		auto bbox_diagonal1 = Math::aabb(Q, Q + u + v);
		auto bbox_diagonal2 = Math::aabb(Q + u, Q + v);
		bbox_ = Math::aabb(bbox_diagonal1, bbox_diagonal2);
	}

	Math::aabb quad::bounding_box() const { return bbox; }

	bool quad::hit(const MAth::ray& r, Math::interval ray_t, hit_record& rec) const {
		return false; // To be implemented;
	}
}