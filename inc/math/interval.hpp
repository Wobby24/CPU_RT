#pragma once

#include "util/common.hpp"

namespace Aero::RT::Math {

	class interval {
	public:
		double min, max;

		interval();
		interval(double min, double max);
		interval(const interval& a, const interval& b);

		double size() const;
		bool contains(double x) const;
		bool surrounds(double x) const;
		double clamp(double x) const;
		interval expand(double delta) const;

		static const interval empty;
		static const interval universe;
	};

	interval operator+(const interval& ival, double displacement);

	interval operator+(double displacement, const interval& ival);
}