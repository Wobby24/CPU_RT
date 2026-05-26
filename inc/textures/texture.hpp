#pragma once

#include "util/color.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Textures {

	class texture {
	public:
		virtual ~texture() = default;

		virtual Utils::color value(double u, double v, const Math::point3& p) const = 0;
	};
}