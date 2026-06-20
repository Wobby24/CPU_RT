#pragma once

#include <cmath>
#include <iostream>
#include "util/common.hpp"

namespace Aero::RT::Math {
	template <typename T>
	//vec4 will use a template to ensure we can use different types instead of a double
	class vec4 {
	public:
		T e[4];

		vec4() : e{ T{}, T{}, T{}, T{} } {}
		vec4(T e0, T e1, T e2, T e3) : e{ e0, e1, e2, e3 } {}
		// allow for intializing with 3 elements to ensure we can maintain compatiblity with vec3
		vec4(T e0, T e1, T e2) : e{ e0, e1, e2, 0 } {}

		T x() const { return e[0]; }
		T y() const { return e[1]; }
		T z() const { return e[2]; }
		T w() const { return e[3]; }

		vec4 operator-() const { return vec4(-e[0], -e[1], -e[2], -e[3]); }
		T operator[](int i) const { return e[i]; }
		T& operator[](int i) { return e[i]; }

		vec4& operator+=(const vec4& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			e[3] += v.e[3];
			return *this;
		}

		vec4& operator-=(const vec4& v) {
			e[0] -= v.e[0];
			e[1] -= v.e[1];
			e[2] -= v.e[2];
			e[3] -= v.e[3];
			return *this;
		}

		vec4& operator*=(T t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			e[3] *= t;
			return *this;
		}

		vec4& operator/=(T t) {
			e[0] /= t;
			e[1] /= t;
			e[2] /= t;
			e[3] /= t;
			return *this;
		}

		double length_squared() const {
			return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
		}

		double length() const {
			return std::sqrt(length_squared());
		}

		static vec4 random() {
			return vec4(random_value<T>(), random_value<T>(), random_value<T>(), random_value<T>());
		}

		static vec4 random(double min, double max) {
			return vec4(random_value<T>(min, max), random_value<T>(min, max), random_value<T>(min, max), random_value<T>(min, max));
		}

		bool near_zero() const {
			// Return true if the vector is close to zero in all dimensions.
			auto s = 1e-8;
			return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s) && (std::fabs(e[3]) < s);
		}
	};

	// Vector Utility Functions (double)

	inline std::ostream& operator<<(std::ostream& out, const vec4<double>& v) {
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
	}

	inline vec4<double> operator+(const vec4<double>& u, const vec4<double>& v) {
		return vec4(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2], u.e[3] + v.e[3]);
	}

	inline vec4<double> operator-(const vec4<double>& u, const vec4<double>& v) {
		return vec4(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
	}

	inline vec4<double> operator*(const vec4<double>& u, const vec4<double>& v) {
		return vec4(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2], u.e[3] * v.e[3]);
	}

	inline vec4<double> operator*(double t, const vec4<double>& v) {
		return vec4(t * v.e[0], t * v.e[1], t * v.e[2], t * v.e[3]);
	}

	inline vec4<double> operator*(const vec4<double>& v, double t) {
		return t * v;
	}

	inline vec4<double> operator/(const vec4<double>& v, double t) {
		return (1 / t) * v;
	}

	inline double dot(const vec4<double>& u, const vec4<double>& v) {
		return u.e[0] * v.e[0]
			+ u.e[1] * v.e[1]
			+ u.e[2] * v.e[2]
			+ u.e[3] * v.e[3];
	}

	// cross product is a 3d operation, so we ignore w
	inline vec4<double> cross(const vec4<double>& u, const vec4<double>& v) {
		return vec4<double>(u.e[1] * v.e[2] - u.e[2] * v.e[1],
			u.e[2] * v.e[0] - u.e[0] * v.e[2],
			u.e[0] * v.e[1] - u.e[1] * v.e[0], 0);
	}

	inline vec4<double> unit_vector(const vec4<double>& v) {
		return v / v.length();
	}

	inline vec4<double> random_in_unit_disk4() {
		auto r = std::sqrt(random_value<double>(0.0, 1.0));
		auto theta = random_value<double>(0.0, 2.0 * 3.141592653589793);
		return vec4<double>(r * std::cos(theta), r * std::sin(theta), 0.0, 0.0);
	}

	inline vec4<double> random_unit_vector4() {
		auto z = random_value<double>(-1.0, 1.0);
		auto a = random_value<double>(0.0, 2.0 * 3.141592653589793);
		auto r = std::sqrt(1.0 - z * z);
		return vec4<double>(r * std::cos(a), r * std::sin(a), z, 0.0);
	}

	inline vec4<double> random_on_hemisphere(const vec4<double>& normal) {
		vec4<double> on_unit_sphere = random_unit_vector4();
		if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}

	inline vec4<double> reflect(const vec4<double>& v, const vec4<double>& n) {
		return v - 2 * dot(v, n) * n;
	}

	inline vec4<double> refract(const vec4<double>& uv, const vec4<double>& n, double etai_over_etat) {
		auto cos_theta = std::fmin(dot(-uv, n), 1.0);
		vec4<double> r_out_perp = etai_over_etat * (uv + cos_theta * n);
		vec4<double> r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
		return r_out_perp + r_out_parallel;
	}
}
