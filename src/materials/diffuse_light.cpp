#include "materials/diffuse_light.hpp"
#include "materials/material.hpp"
#include "textures/texture.hpp"
#include "textures/solid_color.hpp"
#include "math/vec3.hpp"
#include "util/color.hpp"
#include <memory>

namespace Aero::RT::Materials {
	diffuse_light::diffuse_light(std::shared_ptr<Textures::texture> tex) : tex_(tex) {}
	diffuse_light::diffuse_light(const Utils::color& emit) : tex_(std::make_shared<Textures::solid_color>(emit)) {}

	Utils::color diffuse_light::emitted(double u, double v, const Math::point3& p) const {
		return tex_->value(u, v, p);
	}

	bool diffuse_light::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
		return false;
	}
}