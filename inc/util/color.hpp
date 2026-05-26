#pragma once 

#include "math/vec3.hpp"
#include "math/interval.hpp"
#include <cstdint>
#include <iostream>

namespace Aero::RT::Utils {
	using color = Math::vec3<double>;

	double linear_to_gamma(double linear_component);

	void write_color(std::ostream& out, const color& pixel_color);

	uint32_t toRGBA8(float r, float g, float b, float a = 1.0f);
}