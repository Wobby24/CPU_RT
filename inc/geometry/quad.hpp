#pragma once

#include "hittable.hpp"
#include "math/vec3.hpp"
#include "math/aabb.hpp"
#include "math/ray.hpp"
#include "math/interval.hpp"
#include "materials/material.hpp"

#include <memory>

namespace Aero::RT::Geometry {
    class quad : public hittable {
    public:
        quad(const Math::point3& Q, const Math::vec3<double>& u, const Math::vec3<double>& v, std::shared_ptr<Materials::material> mat);
        virtual void set_bounding_box();
        Math::aabb bounding_box() const override;
        bool hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const override;
        virtual bool is_interior(double a, double b, hit_record& rec) const;
    private:
        Math::point3 Q_;
        Math::vec3<double> u_, v_;
        Math::vec3<double> w_;
        std::shared_ptr<Materials::material> mat_;
        Math::aabb bbox_;
        Math::vec3<double> normal_;
        double D_;
    };
}