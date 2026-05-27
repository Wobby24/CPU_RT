#pragma once

#include "math/vec3.hpp"

namespace Aero::RT::Utils {
	class perlin {
	public:
		perlin();
		double noise(const Math::point3& p) const;
		double turb(const Math::point3& p, int depth) const;
	private:
		static const int point_count_ = 256;
		Math::vec3<double> randvec_[point_count_];
		int perm_x_[point_count_];
		int perm_y_[point_count_];
		int perm_z_[point_count_];

		static void perlin_generate_perm(int* p);
		static void permute(int* p, int n);
		static double perlin_interp(const Math::vec3<double> c[2][2][2], double u, double v, double w);
	};
}