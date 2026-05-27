#include "util/perlin_noise.hpp"
#include "textures/texture.hpp"
#include "util/color.hpp"

namespace Aero::RT::Textures {
	class noise_texture : public texture {
	public:
		noise_texture(double scale);

		Utils::color value(double u, double v, const Math::point3& p) const override;
	private:
		Utils::perlin noise_;
		double scale;
	};
}