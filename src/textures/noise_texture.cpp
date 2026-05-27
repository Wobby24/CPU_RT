#include "textures/noise_texture.hpp"
#include "util/color.hpp"

namespace Aero::RT::Textures {
	noise_texture::noise_texture(double scale) : scale(scale) {}

	Utils::color noise_texture::value(double u, double v, const Math::point3& p) const {
		return Utils::color(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise_.turb(p, 7)));
	}
}