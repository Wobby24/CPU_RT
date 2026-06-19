#pragma once

#include <cmath>
#include <iostream>
#include "util/common.hpp"
// Intrinsics header for x64
#include <immintrin.h>

namespace Aero::RT::Simd {
	// align to ensure class is cache "efficient"
	class alignas(32) vec4_simd {
	public:
		// cannot use intializer list for anonymous union members, so instead we just directly assign values to them in the constructor 
		vec4_simd() {
			var[0] = _mm_setzero_pd();
			var[1] = _mm_setzero_pd();
		}
		vec4_simd(double x_val, double y_val, double z_val, double w_val) {
			var[0] = _mm_set_pd(y_val, x_val); // note: set_pd takes arguments in reverse (high, low)
			var[1] = _mm_set_pd(w_val, z_val);
		}

		// for variables in the high lane, we need to shuffle (swap) them to the low lane, which is the only lane _mm_cvtsd_f64 reads from
		double x() const { return _mm_cvtsd_f64(var[0]); }
		// swap the x and y values in the register, the macro at the end is a mask macro, and it tells the cpu which lanes to copy.
		double y() const { return _mm_cvtsd_f64(_mm_shuffle_pd(var[0], var[0], _MM_SHUFFLE2(0, 1))); }
		double z() const { return _mm_cvtsd_f64(var[1]); }
		double w() const { return _mm_cvtsd_f64(_mm_shuffle_pd(var[1], var[1], _MM_SHUFFLE2(0, 1))); }

		vec4_simd operator-() const { return vec4_simd(-x(), -y(), -z(), -w()); }

		// we dont need laod or store because all of our data is already in SIMD variable types, you only need those functions if we need to read or write to a regular memory block (non simd)
		vec4_simd& operator+=(const vec4_simd& v) {
			var[0] = _mm_add_pd(var[0], v.var[0]);
			var[1] = _mm_add_pd(var[1], v.var[1]);
			return *this;
		}

		vec4_simd& operator-=(const vec4_simd& v) {
			var[0] = _mm_sub_pd(var[0], v.var[0]);
			var[1] = _mm_sub_pd(var[1], v.var[1]);
			return *this;
		}

		vec4_simd& operator*=(double scalar) {
			__m128d setScalar = _mm_set1_pd(scalar);
			var[0] = _mm_mul_pd(var[0], setScalar);
			var[1] = _mm_mul_pd(var[1], setScalar);
			return *this;
		}

		vec4_simd& operator*=(const vec4_simd& v) {
			var[0] = _mm_mul_pd(var[0], v.var[0]);
			var[1] = _mm_mul_pd(var[1], v.var[1]);
			return *this;
		}

		vec4_simd& operator/=(double scalar) {
			__m128d setScalar = _mm_set1_pd(scalar);
			var[0] = _mm_div_pd(var[0], setScalar);
			var[1] = _mm_div_pd(var[1], setScalar);
			return *this;
		}

		vec4_simd& operator/=(const vec4_simd& v) {
			var[0] = _mm_div_pd(var[0], v.var[0]);
			var[1] = _mm_div_pd(var[1], v.var[1]);
			return *this;
		}

		double length_squared() const {
			__m128d sq0 = _mm_mul_pd(var[0], var[0]);
			__m128d sq1 = _mm_mul_pd(var[1], var[1]);
			__m128d verticalResult = _mm_add_pd(sq0, sq1);
			return _mm_cvtsd_f64(_mm_hadd_pd(verticalResult, verticalResult));
		}

		double length() const {
			__m128d sq0 = _mm_mul_pd(var[0], var[0]);
			__m128d sq1 = _mm_mul_pd(var[1], var[1]);
			__m128d verticalResult = _mm_add_pd(sq0, sq1);
			return _mm_cvtsd_f64(_mm_sqrt_pd(_mm_hadd_pd(verticalResult, verticalResult)));
		}

		static vec4_simd random() {
			return vec4_simd(random_value<double>(), random_value<double>(), random_value<double>(), random_value<double>());
		}

		static vec4_simd random(double min, double max) {
			return vec4_simd(random_value<double>(min, max), random_value<double>(min, max), random_value<double>(min, max), random_value<double>(min, max));
		}

		bool near_zero() const {
			auto s = 1e-8;
			return (std::fabs(x()) < s) && (std::fabs(y()) < s) && (std::fabs(z()) < s) && (std::fabs(w()) < s);
		}
	private:
		__m128d var[2];
	};

	// util functions 
	inline std::ostream& operator<<(std::ostream& out, const vec4_simd& v) {
		return out << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w();
	}

}