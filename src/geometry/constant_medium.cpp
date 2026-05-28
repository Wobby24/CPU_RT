#include "geometry/constant_medium.hpp"
#include "geometry/hittable.hpp"
#include "materials/material.hpp"
#include "materials/isotropic.hpp"
#include "textures/texture.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/interval.hpp"
#include "math/aabb.hpp"
#include "util/color.hpp"
#include <memory>

namespace Aero::RT::Geometry {
    constant_medium::constant_medium(std::shared_ptr<hittable> boundary, double density, std::shared_ptr<Textures::texture> tex) 
        : boundary_(boundary), neg_inv_density_(-1/density), phase_function_(std::make_shared<Materials::isotropic>(tex)) {}

    constant_medium::constant_medium(std::shared_ptr<hittable> boundary, double density, const Utils::color& albedo) 
        : boundary_(boundary), neg_inv_density_(-1/density), phase_function_(std::make_shared<Materials::isotropic>(albedo)) {}

    bool constant_medium::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
        hit_record rec1, rec2;

        if (!boundary_->hit(r, Math::interval::universe, rec1))
            return false;

        if (!boundary_->hit(r, Math::interval(rec1.t+0.0001, infinity), rec2))
            return false;

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t) 
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();
        auto distnace_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density_ * std::log(random_value<double>());

        if (hit_distance > distnace_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = Math::vec3<double>(1, 0, 0); // arbitrary
        rec.front_face = true; // also arbitrary
        rec.mat = phase_function_;

        return true;
    }

    Math::aabb constant_medium::bounding_box() const {
        return boundary_->bounding_box();
    }
}