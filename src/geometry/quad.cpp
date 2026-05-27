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
		auto n = Math::cross(u ,v);
		normal_ = Math::unit_vector(n);
		D_ = Math::dot(normal_, Q_);
		w_ = n / dot(n,n);

		set_bounding_box();
	}

	void quad::set_bounding_box() {
		// CCompute the bounding box of all four vertices
		auto bbox_diagonal1 = Math::aabb(Q_, Q_ + u_ + v_);
		auto bbox_diagonal2 = Math::aabb(Q_ + u_, Q_ + v_);
		bbox_ = Math::aabb(bbox_diagonal1, bbox_diagonal2);
	}

	Math::aabb quad::bounding_box() const { return bbox_; }

	bool quad::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
		auto denom = Math::dot(normal_, r.direction());

		// No hit if the ray is parallel to the plane.
		if (std::fabs(denom) < 1e-8)
			return false;

		// Return false if the hit point parameter t is outisde the ray interval.
		auto t = (D_ - Math::dot(normal_, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		// Determine if the hit point lies within the planar shape using its plane coordinates.
		auto intersection = r.at(t);
		Math::vec3<double> planar_hitpt_vector = intersection - Q_;
		auto alpha = Math::dot(w_, Math::cross(planar_hitpt_vector, v_));
		auto beta = Math::dot(w_, Math::cross(u_, planar_hitpt_vector));

		if (!is_interior(alpha, beta, rec))
			return false;

		// Ray hits 2D shape; set the rest of the hit record and return true
		rec.t = t;
		rec.p = intersection;
		rec.mat = mat_;
		rec.set_face_normal(r, normal_);

		return true;
	}

	bool quad::is_interior(double a, double b, hit_record& rec) const {
		Math::interval unit_interval = Math::interval(0, 1);
		// Given the hit point in plane coordinates, return false if it is outside the 
		// primitive, otherwise set the hit record UV coordinates and return true.
		if (!unit_interval.contains(a) || !unit_interval.contains(b))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
	}
}