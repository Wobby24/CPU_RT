#include "util/color.hpp"
#include "math/vec3.hpp"
#include "math/interval.hpp"
#include <cstdint>
#include <iostream>
#include <algorithm>

namespace Aero::RT::Utils {
	using color = Math::vec3<double>;

	double linear_to_gamma(double linear_component)
	{
		if (linear_component > 0)
			return std::sqrt(linear_component);

		return 0;
	}

//delete clamping to ensure we are doing HDR, not SDR
	void write_color(std::ostream& out, const color& pixel_color) {
		auto r = pixel_color.x();
		auto g = pixel_color.y();
		auto b = pixel_color.z();


		// Apply a linear to gamma transform for gamma 2
		r = linear_to_gamma(r);
		g = linear_to_gamma(g);
		b = linear_to_gamma(b);

		// Translate the [0, 1] component values to the byte range [0, 255].
		static const Math::interval intensity(0.000, 0.999);
		int rbyte = int(255.999 * intensity.clamp(r));
		int gbyte = int(255.999 * intensity.clamp(g));
		int bbyte = int(255.999 * intensity.clamp(b));

		// Write out the pixel color components.
		out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
	}

	uint32_t toRGBA8(float r, float g, float b, float a)
	{
		auto toByte = [](float c)
			{
				c = std::clamp(c, 0.0f, 0.999f);
				return static_cast<uint32_t>(c * 255.0f);
			};

		uint32_t R = toByte(r);
		uint32_t G = toByte(g);
		uint32_t B = toByte(b);
		uint32_t A = toByte(a);

		return (R << 24) | (G << 16) | (B << 8) | A;
	}
}
