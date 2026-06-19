#include "math/ray.hpp"
#include "math/vec3.hpp" 
#include "hwy/highway.h"
namespace Aero::RT::Math {
	ray::ray() {}

	ray::ray(const point3& origin, const vec3<double>& direction, double time)
		: orig(origin), dir(direction), tm(time) {
	}

	ray::ray(const point3& origin, const vec3<double>& direction)
		: ray(origin, direction, 0) {
	}
}