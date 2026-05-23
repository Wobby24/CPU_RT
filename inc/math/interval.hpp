#pragma once

#include "util/common.hpp"

namespace Aero::RT::Math {
	class interval {
	public:
		double min, max;

		interval();

		interval(double min, double max);

		double size() const;
		bool contains(double x) const;
		bool surrounds(double x) const;
		double clamp(double x) const;

		static const interval empty, universe;
	};

	const interval interval::empty = interval(+infinity, -infinity);
	const interval interval::universe = interval(-infinity, +infinity);
}