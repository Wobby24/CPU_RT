#include "material.hpp"
#include "textures/texture.hpp"
#include "textures/solid_color.hpp"
#include "math/vec3.hpp"
#include "util/color.hpp"
#include <memory>

namespace Aero::RT::Materials {
	class diffuse_light : public material {
	public:
		diffuse_light(std::shared_ptr<Textures::texture> tex);
		diffuse_light(const Utils::color& emit);
		Utils::color emitted(double u, double v, const Math::point3& p) const override;
		bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override;
	private:
		std::shared_ptr<Textures::texture> tex_;
	};
}