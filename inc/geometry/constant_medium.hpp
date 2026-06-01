#pragma once

#include "hittable.hpp"
#include "materials/material.hpp"
#include "textures/texture.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/interval.hpp"
#include "math/aabb.hpp"
#include "util/color.hpp"
#include <memory>

namespace Aero::RT::Geometry {
    class constant_medium : public hittable {
    public:
        constant_medium(std::shared_ptr<hittable> boundary, double density, std::shared_ptr<Textures::texture> tex);
        constant_medium(std::shared_ptr<hittable> boundary, double density, const Utils::color& albedo);

        bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override;
        Math::aabb bounding_box() const override;
    private:
        std::shared_ptr<hittable> boundary_;
        double neg_inv_density_;;
        std::shared_ptr<Materials::material> phase_function_;
    };
}