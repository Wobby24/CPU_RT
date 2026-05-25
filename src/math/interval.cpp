#include "math/interval.hpp"
#include "util/common.hpp"

namespace Aero::RT::Math {

	interval::interval()
		: min(+infinity), max(-infinity) {
	}

	interval::interval(double min, double max)
		: min(min), max(max) {
	}

	interval::interval(const interval& a, const interval& b) {
		// Create the interval tightly enclosing the 2 input intervals
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

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

	interval interval::expand(double delta) const {
		auto padding = delta / 2;
		return interval(min - padding, max + padding);
	}

	const interval interval::empty(+infinity, -infinity);
	const interval interval::universe(-infinity, +infinity);

}