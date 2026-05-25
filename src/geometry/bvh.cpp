#include "geometry/bvh.hpp"
#include "math/aabb.hpp"
#include "geometry/hittable.hpp"
#include "geometry/hittable_list.hpp"
#include "util/common.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace Aero::RT::Geometry {
    bvh_node::bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }

    bvh_node::bvh_node(std::vector<std::shared_ptr<hittable>>& objects, size_t start, size_t end) {
        int axis = random_value<int>(0, 2);

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

    }

    bool bvh_node::hit(const Math::ray& r, Math::interval ray_t, hit_record& rec) const {
        if (!bbox.hit(r, ray_t)) 
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, Math::interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    Math::aabb bvh_node::bounding_box() const {
        return bbox;
    }

    bool bvh_node::box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    bool bvh_node::box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    bool bvh_node::box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    bool bvh_node::box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }
}   