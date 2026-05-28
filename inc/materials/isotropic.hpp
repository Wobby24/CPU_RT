#pragma once

#include "util/color.hpp"
#include "math/ray.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include <textures/texture.hpp>
#include <memory>

namespace Aero::RT::Materials {
    class isotropic : public material {
    public:
        isotropic(const Utils::color& albedo);
        isotropic(std::shared_ptr<Textures::texture> tex);

        bool scatter(const Math::ray& r_in, const Geometry::hit_record& rec, Utils::color& attenuation, Math::ray& scattered) const override;
	private:
        std::shared_ptr<Textures::texture> tex_;
    };
}