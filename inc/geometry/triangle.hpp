#include "quad.hpp"
#include "hittable.hpp"
#include "materials/material.hpp"
#include "math/vec3.hpp"
#include <memory>

namespace Aero::RT::Geometry {
	class triangle : public quad {
	public:
		triangle(const Math::point3& o, const Math::vec3<double>& aa, const Math::vec3<double>& ab, std::shared_ptr<Materials::material> m);
		virtual bool is_interior(double a, double b, hit_record& rec) const override;
	};
}