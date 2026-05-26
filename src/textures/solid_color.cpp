#include "textures/solid_color.hpp"
#include "util/color.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Textures {
	solid_color::solid_color(const Utils::color& albedo) : albedo_(albedo) {}
	
	solid_color::solid_color(double red, double green, double blue) : solid_color(Utils::color(red, green, blue)) {}

	Utils::color solid_color::value(double u, double v, const Math::point3& p) const {
		return albedo_;
	}
}