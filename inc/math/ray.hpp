#pragma once 

#include "math/vec3.hpp" 

namespace Aero::RT::Math {
	class ray {
	public:
		ray() {}
		ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

		const piont3& origin() const { return orig; }
		const vec3& direction() const { return dir; }

		point3 at(double t) const {
			return orig + t * dir;
		}

	private:
		point3 orig;
		vec3 dir;
	};
}