#pragma once

#include "texture.hpp"
#include "util/color.hpp"
#include "math/vec3.hpp"

namespace Aero::RT::Textures {
	class solid_color : public texture {
	public:
		solid_color(const Utils::color& albedo);

		solid_color(double red, double green, double blue);

		Utils::color value(double u, double v, const Math::point3& p) const override;
	private:
		Utils::color albedo_;
	};
}