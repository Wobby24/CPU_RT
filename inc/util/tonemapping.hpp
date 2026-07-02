#pragma once
#include "util/color.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Utils {
	Utils::color Reinhard_TMO(Utils::color color) {
		
		color = color / (color + Utils::color(1.0, 1.0, 1.0));
		return color;
	}
}