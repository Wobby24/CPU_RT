#include "materials/isotropic.hpp"
#include "util/color.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "textures/texture.hpp"
#include "textures/solid_color.hpp"
#include <memory>

namespace Aero::RT::Materials {
    isotropic::isotropic(const Utils::color& albedo) 
        : tex_(std::make_shared<Textures::solid_color>(albedo)) {}

    isotropic::isotropic(std::shared_ptr<Textures::texture> tex) 
        : tex_(tex) {}

    bool isotropic::scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const {
        scattered = Math::ray(rec.p, Math::random_unit_vector(), r_in.time());
        attenuation = tex_->value(rec.u, rec.v, rec.p);
        return true;
    }
}