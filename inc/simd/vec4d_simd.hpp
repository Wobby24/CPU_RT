#pragma once

#include <cmath>
#include <iostream>
#include "util/common.hpp"
// Intrinsics header for x64
#include <immintrin.h>

namespace Aero::RT::Simd {
	// align to ensure class is cache "efficient", change alignment from 16 to 32 when switching to avx2
	class alignas(16) vec4d_simd {
	public:
		// cannot use intializer list for anonymous union members, so instead we just directly assign values to them in the constructor 
		vec4d_simd() {
			var[0] = _mm_setzero_pd();
			var[1] = _mm_setzero_pd();
		}
		vec4d_simd(double x_val, double y_val, double z_val, double w_val) {
			var[0] = _mm_set_pd(y_val, x_val); // note: set_pd takes arguments in reverse (high, low)
			var[1] = _mm_set_pd(w_val, z_val);
		}

		vec4d_simd(__m128d xy_val, __m128d zw_val) {
			var[0] = xy_val;
			var[1] = zw_val;
		}

		//unpack returns the section of the register you want, either hihg or low [high, high] or [low, low]
		// that allows us to do math on both

		// shuffle gives you total control to mix, match, and rearrange the elements however you want. For example, to swap data for a vector with [low, high],m you use _MM_SHUFFLE2(0, 1)) to get [high, low]
		// for variables in the high lane, we need to shuffle (swap) them to the low lane, which is the only lane _mm_cvtsd_f64 reads from
		double x() const { return _mm_cvtsd_f64(var[0]); }
		// swap the x and y values in the register, the macro at the end is a mask macro, and it tells the cpu which lanes to copy.
		double y() const { return _mm_cvtsd_f64(_mm_shuffle_pd(var[0], var[0], _MM_SHUFFLE2(0, 1))); }
		double z() const { return _mm_cvtsd_f64(var[1]); }
		double w() const { return _mm_cvtsd_f64(_mm_shuffle_pd(var[1], var[1], _MM_SHUFFLE2(0, 1))); }

		// simd alternatives for returning values for non member vector functions. Not recommeneded to use these because of performance penalties.
		__m128d x_simd() const { return var[0]; }
		__m128d y_simd() const { return _mm_shuffle_pd(var[0], var[0], _MM_SHUFFLE2(0, 1)); }
		__m128d z_simd() const { return var[1]; }
		__m128d w_simd() const { return _mm_shuffle_pd(var[1], var[1], _MM_SHUFFLE2(0, 1)); }

		// returns the variable, not indivdual value
		__m128d xy_simd() const { return var[0]; }
		__m128d zw_simd() const { return var[1]; }

		// A cleaner, pure-SIMD way to negate without dropping to scalar floats
		vec4d_simd operator-() const {
			// The highest bit of a double represents the sign. 
			// Creating a mask with the sign bit set:
			__m128d sign_mask = _mm_set1_pd(-0.0);

			// XORing any float/double with this mask flips its sign instantly
			return vec4d_simd(_mm_xor_pd(var[0], sign_mask), _mm_xor_pd(var[1], sign_mask));
		}

		// we dont need laod or store because all of our data is already in SIMD variable types, you only need those functions if we need to read or write to a regular memory block (non simd)
		vec4d_simd& operator+=(const vec4d_simd& v) {
			var[0] = _mm_add_pd(var[0], v.var[0]);
			var[1] = _mm_add_pd(var[1], v.var[1]);
			return *this;
		}

		vec4d_simd& operator-=(const vec4d_simd& v) {
			var[0] = _mm_sub_pd(var[0], v.var[0]);
			var[1] = _mm_sub_pd(var[1], v.var[1]);
			return *this;
		}

		vec4d_simd& operator*=(double scalar) {
			__m128d setScalar = _mm_set1_pd(scalar);
			var[0] = _mm_mul_pd(var[0], setScalar);
			var[1] = _mm_mul_pd(var[1], setScalar);
			return *this;
		}

		vec4d_simd& operator*=(const vec4d_simd& v) {
			var[0] = _mm_mul_pd(var[0], v.var[0]);
			var[1] = _mm_mul_pd(var[1], v.var[1]);
			return *this;
		}

		vec4d_simd& operator/=(double scalar) {
			__m128d setScalar = _mm_set1_pd(scalar);
			var[0] = _mm_div_pd(var[0], setScalar);
			var[1] = _mm_div_pd(var[1], setScalar);
			return *this;
		}

		vec4d_simd& operator/=(const vec4d_simd& v) {
			var[0] = _mm_div_pd(var[0], v.var[0]);
			var[1] = _mm_div_pd(var[1], v.var[1]);
			return *this;
		}

		double length_squared() const {
			// 1. Square the components
			__m128d sq0 = _mm_mul_pd(var[0], var[0]); // [x^2, y^2]
			__m128d sq1 = _mm_mul_pd(var[1], var[1]); // [z^2, w^2]

			// 2. Add the two vectors together
			__m128d sum = _mm_add_pd(sq0, sq1);       // [x^2 + z^2, y^2 + w^2]

			// 3. Move the high part (y^2 + w^2) to the low part of a temporary register
			__m128d high = _mm_unpackhi_pd(sum, sum);

			// 4. Add the low and high parts together
			__m128d total = _mm_add_sd(sum, high);    // Low slot now has x^2 + z^2 + y^2 + w^2

			// 5. Square root of just the scalar lower slot
			return _mm_cvtsd_f64(total);
		}

		double length() const {
			// 1. Square the components
			__m128d sq0 = _mm_mul_pd(var[0], var[0]); // [x^2, y^2]
			__m128d sq1 = _mm_mul_pd(var[1], var[1]); // [z^2, w^2]

			// 2. Add the two vectors together
			__m128d sum = _mm_add_pd(sq0, sq1);       // [x^2 + z^2, y^2 + w^2]

			// 3. Move the high part (y^2 + w^2) to the low part of a temporary register
			__m128d high = _mm_unpackhi_pd(sum, sum);

			// 4. Add the low and high parts together
			__m128d total = _mm_add_sd(sum, high);    // Low slot now has x^2 + z^2 + y^2 + w^2

			// 5. Square root of just the scalar lower slot
			return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), total));
		}

		static vec4d_simd random() {
			return vec4d_simd(random_value<double>(), random_value<double>(), random_value<double>(), random_value<double>());
		}

		static vec4d_simd random(double min, double max) {
			return vec4d_simd(random_value<double>(min, max), random_value<double>(min, max), random_value<double>(min, max), random_value<double>(min, max));
		}

		bool near_zero() const {
			auto s = 1e-8;
			return (std::fabs(x()) < s) && (std::fabs(y()) < s) && (std::fabs(z()) < s) && (std::fabs(w()) < s);
		}
	private:
		__m128d var[2];
	};

	// util functions 
	inline std::ostream& operator<<(std::ostream& out, const vec4d_simd& v) {
		return out << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w();
	}

	inline vec4d_simd operator+(const vec4d_simd& u, const vec4d_simd& v) {
		__m128d newXY = _mm_add_pd(u.xy_simd(), v.xy_simd());
		__m128d newZW = _mm_add_pd(u.zw_simd(), v.zw_simd());

		return vec4d_simd(newXY, newZW);
	}

	inline vec4d_simd operator-(const vec4d_simd& u, const vec4d_simd& v) {
		__m128d newXY = _mm_sub_pd(u.xy_simd(), v.xy_simd());
		__m128d newZW = _mm_sub_pd(u.zw_simd(), v.zw_simd());

		return vec4d_simd(newXY, newZW);
	}

	inline vec4d_simd operator*(const vec4d_simd& u, const vec4d_simd& v) {
		__m128d newXY = _mm_mul_pd(u.xy_simd(), v.xy_simd());
		__m128d newZW = _mm_mul_pd(u.zw_simd(), v.zw_simd());

		return vec4d_simd(newXY, newZW);
	}

	inline vec4d_simd operator*(double t, const vec4d_simd& v) {
		__m128d setScalar = _mm_set1_pd(t);
		__m128d newXY = _mm_mul_pd(v.xy_simd(), setScalar);
		__m128d newZW = _mm_mul_pd(v.zw_simd(), setScalar);

		return vec4d_simd(newXY, newZW);
	}

	inline vec4d_simd operator*(const vec4d_simd& v, double t) {
		__m128d setScalar = _mm_set1_pd(t);
		__m128d newXY = _mm_mul_pd(v.xy_simd(), setScalar);
		__m128d newZW = _mm_mul_pd(v.zw_simd(), setScalar);

		return vec4d_simd(newXY, newZW);
	}

	inline vec4d_simd operator/(const vec4d_simd& v, double t) {
		__m128d setScalar = _mm_set1_pd(t);
		__m128d newXY = _mm_div_pd(v.xy_simd(), setScalar);
		__m128d newZW = _mm_div_pd(v.zw_simd(), setScalar);

		return vec4d_simd(newXY, newZW);
	}

	// super inefficient. surely there is a better way
	inline double dot(const vec4d_simd& u, const vec4d_simd& v) {
	//	return u.x() * v.x() + u.y() * v.y() + u.z() * v.z() + u.w() * v.w();
		// square everything
		__m128d mulXY = _mm_mul_pd(u.xy_simd(), v.xy_simd()); // [u.x*v.x, u.y*v.y]
		__m128d mulZW = _mm_mul_pd(u.zw_simd(), v.zw_simd()); // [u.z*v.z, u.w*v.w]

		__m128d verticalResult = _mm_add_pd(mulXY, mulZW);

		// add up mulxy and zw with vertical then horizontally add those up
		return _mm_cvtsd_f64(_mm_hadd_pd(verticalResult, verticalResult));
	}

	// cross product is a 3d operation, so we ignore w
	//_mm_unpacklo_pd gives you the first elements : [A0, B0]

	//_mm_unpackhi_pd gives you the second elements : [A1, B1]
	inline vec4d_simd cross(const vec4d_simd& u, const vec4d_simd& v) {
		__m128d u_xy = u.xy_simd(); 
		__m128d u_zw = u.zw_simd();

		__m128d v_xy = v.xy_simd(); 
		__m128d v_zw = v.zw_simd(); 

		// we need [ (uy * vz) - (uz * vy), (uz * vx) - (ux * vz) ]

		// Left side elements: [uy, uz]
		__m128d u_yz = _mm_unpackhi_pd(u_xy, u_zw);
		// Right side elements: [vz, vx]
		__m128d v_zx = _mm_shuffle_pd(v_zw, v_xy, _MM_SHUFFLE2(0, 0));

		// Left side elements: [uz, ux]
		__m128d u_zx = _mm_shuffle_pd(u_zw, u_xy, _MM_SHUFFLE2(0, 0));
		// Right side elements: [vy, vz]
		__m128d v_yz = _mm_unpackhi_pd(v_xy, v_zw);

		// Multiply and subtract for the first two components
		__m128d res_xy = _mm_sub_pd(_mm_mul_pd(u_yz, v_zx), _mm_mul_pd(u_zx, v_yz));

		// we need [ (ux * vy) - (uy * vx), 0 ]

		// Left side elements: [ux, 0]
		__m128d u_x0 = _mm_unpacklo_pd(u_xy, _mm_setzero_pd());
		// Right side elements: [vy, 0]
		__m128d v_y0 = _mm_unpackhi_pd(v_xy, _mm_setzero_pd());

		// Left side elements: [uy, 0]
		__m128d u_y0 = _mm_unpackhi_pd(u_xy, _mm_setzero_pd());
		// Right side elements: [vx, 0]
		__m128d v_x0 = _mm_unpacklo_pd(v_xy, _mm_setzero_pd());

		// Multiply and subtract for the final components
		__m128d res_zw = _mm_sub_pd(_mm_mul_pd(u_x0, v_y0), _mm_mul_pd(u_y0, v_x0));

		return vec4d_simd(res_xy, res_zw);
	}

	inline vec4d_simd unit_vector(const vec4d_simd& v) {
		return v / v.length();
	}

	inline vec4d_simd random_in_unit_disk4() {
		auto r = std::sqrt(random_value<double>(0.0, 1.0));
		auto theta = random_value<double>(0.0, 2.0 * 3.141592653589793);
		return vec4d_simd(r * std::cos(theta), r * std::sin(theta), 0.0, 0.0);
	}

	inline vec4d_simd random_unit_vector4() {
		auto z = random_value<double>(-1.0, 1.0);
		auto a = random_value<double>(0.0, 2.0 * 3.141592653589793);
		auto r = std::sqrt(1.0 - z * z);
		return vec4d_simd(r * std::cos(a), r * std::sin(a), z, 0.0);
	}

	inline vec4d_simd random_on_hemisphere(const vec4d_simd& normal) {
		vec4d_simd on_unit_sphere = random_unit_vector4();
		if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}

	inline vec4d_simd reflect(const vec4d_simd& v, const vec4d_simd& n) {
		return v - 2 * dot(v, n) * n;
	}

	inline vec4d_simd refract(const vec4d_simd& uv, const vec4d_simd& n, double etai_over_etat) {
		auto cos_theta = std::fmin(dot(-uv, n), 1.0);
		vec4d_simd r_out_perp = etai_over_etat * (uv + cos_theta * n);
		vec4d_simd r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
		return r_out_perp + r_out_parallel;
	}

}