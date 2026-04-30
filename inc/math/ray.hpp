#pragma once 

#include "math/vec3.hpp" 

namespace Aero::RT::Math {
	class ray {
	public:
		ray() {}
		ray(const Aero::RT::Math::point3& origin, const Aero::RT::Math::vec3<double>& direction) : orig(origin), dir(direction) {}

		const Aero::RT::Math::point3& origin() const { return orig; }
		const Aero::RT::Math::vec3<double>& direction() const { return dir; }

		Aero::RT::Math::point3 at(double t) const {
			return orig + t * dir;
		}

	private:
		Aero::RT::Math::point3 orig;
		Aero::RT::Math::vec3<double> dir;
	};
}