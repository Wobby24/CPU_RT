#pragma once

#include <cmath>
#include <iostream>
#include "util/common.hpp"
// Intrinsics header for x64
#include <immintrin.h>

namespace Aero::RT::Simd {
	// align to ensure class is cache "efficient", change alignment from 16 to 32 when switching to avx2
	class alignas(16) vec4f_simd {
	public:
		// cannot use intializer list for anonymous union members, so instead we just directly assign values to them in the constructor 
		vec4f_simd() {
			var = _mm_setzero_ps();
		}
		vec4f_simd(float x_val, float y_val, float z_val, float w_val) {
			var = _mm_set_ps(w_val, z_val, y_val, x_val); // note: set_pd takes arguments in reverse (high, low)
		}

		vec4f_simd(__m128 val) {
			var = val;
		}

		//unpack returns the section of the register you want, either hihg or low [high, high] or [low, low]
		// that allows us to do math on both

		// shuffle gives you total control to mix, match, and rearrange the elements however you want. For example, to swap data for a vector with [low, high],m you use _MM_SHUFFLE2(0, 1)) to get [high, low]
		// for variables in the high lane, we need to shuffle (swap) them to the low lane, which is the only lane _mm_cvtss_f32 reads from
		float x() const { return _mm_cvtss_f32(var); }
		// swap the x and y values in the register, the macro at the end is a mask macro, and it tells the cpu which lanes to copy. the shuffle arguement number (which argument, not what we pass to it) is the destination lane number.
		// first arugment will be lane 3, second lane 2, third lane 1, and last one is lane 0
		float y() const {
			// 4th number is 1 -> Grabs Y and puts it in Lane 0
			// Y is stored in lane 1, and so we put it lane 0, hencc e we can read it normally
			return _mm_cvtss_f32(_mm_shuffle_ps(var, var, _MM_SHUFFLE(1, 1, 1, 1)));
		}

		float z() const {
			// 4th number is 2 -> Grabs Z and puts it in Lane 0
			return _mm_cvtss_f32(_mm_shuffle_ps(var, var, _MM_SHUFFLE(2, 2, 2, 2)));
		}

		float w() const {
			// 4th number is 3 -> Grabs W and puts it in Lane 0
			return _mm_cvtss_f32(_mm_shuffle_ps(var, var, _MM_SHUFFLE(3, 3, 3, 3)));
		}

		__m128 xy_simd() const {
			// _mm_movelh_ps copies the lower 2 lanes of the second vector 
			// into the upper 2 lanes of the first vector.
			// Result: (x, y, x, y)
			return _mm_movelh_ps(var, var);
		}

		// Returns (z, w, z, w)
		__m128 zw_simd() const {
			// _mm_movehl_ps copies the upper 2 lanes of the second vector 
			// into the lower 2 lanes of the first vector.
			// Result: (z, w, z, w)
			return _mm_movehl_ps(var, var);
		}

		__m128 xyzw_simd() const { return var; }

		// A cleaner, pure-SIMD way to negate without dropping to scalar floats
		vec4f_simd operator-() const {
			// The highest bit of a float represents the sign. 
			// Creating a mask with the sign bit set:
			__m128 sign_mask = _mm_set1_ps(-0.0);

			// XORing any float/float with this mask flips its sign instantly
			return vec4f_simd(_mm_xor_ps(var, sign_mask));
		}

		// we dont need laod or store because all of our data is already in SIMD variable types, you only need those functions if we need to read or write to a regular memory block (non simd)
		vec4f_simd& operator+=(const vec4f_simd& v) {
			var = _mm_add_ps(var, v.var);
			return *this;
		}

		vec4f_simd& operator-=(const vec4f_simd& v) {
			var = _mm_sub_ps(var, v.var);
			return *this;
		}

		vec4f_simd& operator*=(float scalar) {
			__m128 setScalar = _mm_set1_ps(scalar);
			var = _mm_mul_ps(var, setScalar);
			return *this;
		}

		vec4f_simd& operator*=(const vec4f_simd& v) {
			var = _mm_mul_ps(var, v.var);
			return *this;
		}

		vec4f_simd& operator/=(float scalar) {
			__m128 setScalar = _mm_set1_ps(scalar);
			var = _mm_div_ps(var, setScalar);
			return *this;
		}

		vec4f_simd& operator/=(const vec4f_simd& v) {
			var = _mm_div_ps(var, v.var);
			return *this;
		}

		float length_squared() const {
			// 1. Square the components
			__m128 total = _mm_mul_ps(var, var); // [x^2, y^2, z^2, w^2]

			// 5. Square root of just the scalar lower slot
			return _mm_cvtss_f32(total);
		}

		float length() const {
			// 1. Square the components
			__m128 total = _mm_mul_ps(var, var); // [x^2, y^2, z^2, w^2]

			// 5. Square root of just the scalar lower slot
			return _mm_cvtss_f32(_mm_sqrt_ss(total));
		}

		static vec4f_simd random() {
			return vec4f_simd(random_value<float>(), random_value<float>(), random_value<float>(), random_value<float>());
		}

		static vec4f_simd random(float min, float max) {
			return vec4f_simd(random_value<float>(min, max), random_value<float>(min, max), random_value<float>(min, max), random_value<float>(min, max));
		}

		bool near_zero() const {
			auto s = 1e-8;
			return (std::fabs(x()) < s) && (std::fabs(y()) < s) && (std::fabs(z()) < s) && (std::fabs(w()) < s);
		}
	private:
		__m128 var;
	};

	// util functions 
	inline std::ostream& operator<<(std::ostream& out, const vec4f_simd& v) {
		return out << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w();
	}

	inline vec4f_simd operator+(const vec4f_simd& u, const vec4f_simd& v) {
		__m128 newXYZW = _mm_add_ps(u.xyzw_simd(), v.xyzw_simd());

		return vec4f_simd(newXYZW);
	}

	inline vec4f_simd operator-(const vec4f_simd& u, const vec4f_simd& v) {
		__m128 newXYZW = _mm_sub_ps(u.xyzw_simd(), v.xyzw_simd());

		return vec4f_simd(newXYZW);
	}

	inline vec4f_simd operator*(const vec4f_simd& u, const vec4f_simd& v) {
		__m128 newXYZW = _mm_mul_ps(u.xyzw_simd(), v.xyzw_simd());
		return vec4f_simd(newXYZW);
	}

	inline vec4f_simd operator*(float t, const vec4f_simd& v) {
		__m128 setScalar = _mm_set1_ps(t);
		__m128 newXYZW = _mm_mul_ps(v.xyzw_simd(), setScalar);

		return vec4f_simd(newXYZW);
	}

	inline vec4f_simd operator*(const vec4f_simd& v, float t) {
		__m128 setScalar = _mm_set1_ps(t);
		__m128 newXYZW = _mm_mul_ps(v.xyzw_simd(), setScalar);

		return vec4f_simd(newXYZW);
	}


	inline vec4f_simd operator/(const vec4f_simd& v, float t) {
		__m128 setScalar = _mm_set1_ps(t);
		__m128 newXYZW = _mm_div_ps(v.xyzw_simd(), setScalar);

		return vec4f_simd(newXYZW);
	}

	// super inefficient. surely there is a better way. SSE 4.1 has a dot product method
	inline float dot(const vec4f_simd& u, const vec4f_simd& v) {
		//	return u.x() * v.x() + u.y() * v.y() + u.z() * v.z() + u.w() * v.w();
			// square everything
		__m128 mulXYZW = _mm_mul_ps(u.xyzw_simd(), v.xyzw_simd());
		__m128 sum = _mm_hadd_ps(mulXYZW, mulXYZW); // [X+Y, Z+W, X+Y, Z+W]
		// add up mulxy and zw with vertical then horizontally add those up
		return _mm_cvtss_f32(_mm_hadd_ps(sum, sum));
	}

	// cross product is a 3d operation, so we ignore w
	//_mm_unpacklo_pd gives you the first elements : [A0, B0]

	//_mm_unpackhi_pd gives you the second elements : [A1, B1]
	// super inefficient
	inline vec4f_simd cross(const vec4f_simd& u, const vec4f_simd& v) {
		__m128 u_xy = u.xy_simd();
		__m128 u_zw = u.zw_simd();

		__m128 v_xy = v.xy_simd();
		__m128 v_zw = v.zw_simd();

		// we need [ (uy * vz) - (uz * vy), (uz * vx) - (ux * vz) ]

		// Left side elements: [uy, uz]
		__m128 u_yz = _mm_unpackhi_ps(u_xy, u_zw);
		// Right side elements: [vz, vx]
		__m128 v_zx = _mm_shuffle_ps(v_zw, v_xy, _MM_SHUFFLE2(0, 0));

		// Left side elements: [uz, ux]
		__m128 u_zx = _mm_shuffle_ps(u_zw, u_xy, _MM_SHUFFLE2(0, 0));
		// Right side elements: [vy, vz]
		__m128 v_yz = _mm_unpackhi_ps(v_xy, v_zw);

		// Multiply and subtract for the first two components
		__m128 res_xy = _mm_sub_ps(_mm_mul_ps(u_yz, v_zx), _mm_mul_ps(u_zx, v_yz));

		// we need [ (ux * vy) - (uy * vx), 0 ]

		// Left side elements: [ux, 0]
		__m128 u_x0 = _mm_unpacklo_ps(u_xy, _mm_setzero_ps());
		// Right side elements: [vy, 0]
		__m128 v_y0 = _mm_unpackhi_ps(v_xy, _mm_setzero_ps());

		// Left side elements: [uy, 0]
		__m128 u_y0 = _mm_unpackhi_ps(u_xy, _mm_setzero_ps());
		// Right side elements: [vx, 0]
		__m128 v_x0 = _mm_unpacklo_ps(v_xy, _mm_setzero_ps());

		// Multiply and subtract for the final components
		__m128 res_zw = _mm_sub_ps(_mm_mul_ps(u_x0, v_y0), _mm_mul_ps(u_y0, v_x0));

		return vec4f_simd(_mm_add_ps(res_xy, res_zw));
	}

	inline vec4f_simd unit_vector(const vec4f_simd& v) {
		return v / v.length();
	}

	inline vec4f_simd random_in_unit_diskf() {
		auto r = std::sqrt(random_value<float>(0.0, 1.0));
		auto theta = random_value<float>(0.0, 2.0 * 3.141592653589793);
		return vec4f_simd(r * std::cos(theta), r * std::sin(theta), 0.0, 0.0);
	}

	inline vec4f_simd random_unit_vectorf() {
		auto z = random_value<float>(-1.0, 1.0);
		auto a = random_value<float>(0.0, 2.0 * 3.141592653589793);
		auto r = std::sqrt(1.0 - z * z);
		return vec4f_simd(r * std::cos(a), r * std::sin(a), z, 0.0);
	}

	inline vec4f_simd random_on_hemisphere(const vec4f_simd& normal) {
		vec4f_simd on_unit_sphere = random_unit_vectorf();
		if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}

	inline vec4f_simd reflect(const vec4f_simd& v, const vec4f_simd& n) {
		return v - 2 * dot(v, n) * n;
	}

	inline vec4f_simd refract(const vec4f_simd& uv, const vec4f_simd& n, float etai_over_etat) {
		auto cos_theta = std::fmin(dot(-uv, n), 1.0);
		vec4f_simd r_out_perp = etai_over_etat * (uv + cos_theta * n);
		vec4f_simd r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
		return r_out_perp + r_out_parallel;
	}

}