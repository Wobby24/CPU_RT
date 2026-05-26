#include "geometry/sphere.hpp"
#include "math/aabb.hpp"
#include <memory>

namespace Aero::RT::Geometry {
	//Static sphere 
	sphere::sphere(const Math::point3& static_center, double radius, std::shared_ptr<Materials::material> mat) :
	 center_(static_center, Math::vec3<double>(0,0,0)), radius_(std::fmax(0, radius)), mat_(mat) 
	 {
		auto rvec = Math::vec3<double>(radius, radius, radius);
		bbox = Math::aabb(static_center - rvec, static_center + rvec);
	 }

	// Moving sphere, center 1 and center 2 is its starting and end position
	sphere::sphere(const Math::point3& center1, const Math::point3& center2, double radius,
		std::shared_ptr<Materials::material> mat)
		: center_(center1, center2 - center1), radius_(std::fmax(0, radius)), mat_(mat) 
	{
		auto rvec = Math::vec3<double>(radius, radius, radius);
		Math::aabb box1(center_.at(0) - rvec, center_.at(0) + rvec);
		Math::aabb box2(center_.at(1) - rvec, center_.at(1) + rvec);
		bbox = Math::aabb(box1, box2);
	}


	 bool sphere::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
			Math::point3 current_center = center_.at(r.time());
			Math::vec3<double> oc = current_center - r.origin();
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
				Math::vec3<double> outward_normal = (rec.p - current_center) / radius_;
				rec.set_face_normal(r, outward_normal);
				get_sphere_uv(outward_normal, rec.u, rec.v);
				rec.mat = mat_;

				return true;
		}

	Math::aabb sphere::bounding_box() const {
		return bbox;
	}

	void sphere::get_sphere_uv(const Math::point3& p, double& u, double& v) {
		        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
	}
}