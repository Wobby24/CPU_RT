#pragma once 

#include "math/vec3.hpp"
#include "math/interval.hpp"

#include <iostream>

namespace Aero::RT::Utils {
	using color = Math::vec3<double>;

	void write_color(std::ostream& out, const color& pixel_color) {
		auto r = pixel_color.x();
		auto g = pixel_color.y();
		auto b = pixel_color.z();

		// Translate the [0, 1] component values to the byte range [0, 255].
		static const Math::interval intensity(0.000, 0.999);
		int rbyte = int(255.999 * intensity.clamp(r));
		int gbyte = int(255.999 * intensity.clamp(g));
		int bbyte = int(255.999 * intensity.clamp(b));

		// Write out the pixel color components.
		out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
	}
}