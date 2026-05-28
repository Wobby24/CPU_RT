#include "geometry/triangle.hpp"
#include "geometry/quad.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"
#include <memory>

namespace Aero::RT::Geometry {
	triangle::triangle(const Math::point3& o, const Math::vec3<double>& aa, const Math::vec3<double>& ab, std::shared_ptr<Materials::material> m)
		: quad(o, aa, ab, m) {}

	bool triangle::is_interior(double a, double b, hit_record& rec) const {
		if ((a < 0) || (b < 0) || (a + b > 1))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
	}
}