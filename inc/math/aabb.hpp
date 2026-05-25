#pragma once 
#include "interval.hpp"
#include "vec3.hpp"
#include "ray.hpp"

namespace Aero::RT::Math {
	class aabb {
	public:
		interval x, y, z;

		aabb(); // Default AABB is empty because intervals are empty by default
		aabb(const interval& x, const interval& y, const interval& z);
		aabb(const point3& a, const point3& b);

		const interval& axis_interval(int n) const;
		bool hit(const ray& r, interval ray_t) const;
	};
}