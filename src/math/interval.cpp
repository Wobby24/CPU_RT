#include "math/interval.hpp"
#include "util/common.hpp"

namespace Aero::RT::Math {
	interval::interval() : min(+infinity), max(-infinity) {} // Default interval is empty

	interval::interval(double min, double max) : min(min), max(max) {}

	double interval::size() const {
		return max - min;
	}

	bool interval::contains(double x) const {
		return min <= x && x <= max;
	}

	bool interval::surrounds(double x) const {
		return min < x && x < max;
	}

	double interval::clamp(double x) const {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}
}