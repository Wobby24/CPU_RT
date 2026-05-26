#pragma once

#include "texture.hpp"
#include "util/color.hpp"
#include "math/vec3.hpp"
#include <memory>

namespace Aero::RT::Textures {
	class checker_texture : public texture {
	public:
		checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd);

		checker_texture(double scale, const Utils::color& c1, const Utils::color& c2);

		Utils::color value(double u, double v, const Math::point3& p) const override;
	private:
		double inv_scale;
		std::shared_ptr<texture> even_;
		std::shared_ptr<texture> odd_;
	};
}