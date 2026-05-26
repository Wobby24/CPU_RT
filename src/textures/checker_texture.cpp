#include "textures/checker_texture.hpp"
#include "textures/solid_color.hpp"
#include "textures/texture.hpp"
#include "math/vec3.hpp"
#include "util/color.hpp"
#include <memory>
#include <cmath>

namespace Aero::RT::Textures {
	checker_texture::checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd) : inv_scale(1.0 / scale), even_(even), odd_(odd) {}

	checker_texture::checker_texture(double scale, const Utils::color& c1, const Utils::color& c2) : checker_texture(scale, std::make_shared<solid_color>(c1), std::make_shared<solid_color>(c2)) {}

	Utils::color checker_texture::value(double u, double v, const Math::point3& p) const {
		auto xInteger = int(std::floor(inv_scale * p.x()));
		auto yInteger = int(std::floor(inv_scale * p.y()));
		auto zInteger = int(std::floor(inv_scale * p.z()));

		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

		return isEven ? even_->value(u, v, p) : odd_->value(u, v, p);
	}
}