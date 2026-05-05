#pragma once

#include <cmath>
#include <iostream>
#include "util/common.hpp"

namespace Aero::RT::Math {
	template <typename T>
	//vec3 will use a template to ensure we can use different types instead of a double
	class vec3 {
	public:
		T e[3];

		vec3() : e{ T{}, T{}, T{} } {}
		vec3(T e0, T e1, T e2) : e{ e0, e1, e2 } {}

		T x() const { return e[0]; }
		T y() const { return e[1]; }
		T z() const { return e[2]; }

		vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		T operator[](int i) const { return e[i]; }
		T& operator[](int i) { return e[i]; }

		vec3& operator+=(const vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}

		vec3& operator*=(T t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		vec3& operator/=(T t) {
			return *this *= 1 / t;
		}

		double length_squared() const {
			return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
		}

		double length() const {
			return std::sqrt(length_squared());
		}

		static vec3 random() {
			return vec3(random_value<T>(), random_value<T>(), random_value<T>());
		}

		static vec3 random(double min, double max) {
			return vec3(random_value<T>(min, max), random_value<T>(min, max), random_value<T>(min, max));
		}

		bool near_zero() const {
			// Return true if the vector is close to zero in all dimensions.
			auto s = 1e-8;
			return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
		}
	};

	// point3 is just an alias for vec3, but useful for geometric clarity in the code.
	using point3 = Aero::RT::Math::vec3<double>;


	// Vector Utility Functions (double)

	inline std::ostream& operator<<(std::ostream& out, const vec3<double>& v) {
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	inline vec3<double> operator+(const vec3<double>& u, const vec3<double>& v) {
		return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
	}

	inline vec3<double> operator-(const vec3<double>& u, const vec3<double>& v) {
		return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
	}

	inline vec3<double> operator*(const vec3<double>& u, const vec3<double>& v) {
		return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}

	inline vec3<double> operator*(double t, const vec3<double>& v) {
		return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
	}

	inline vec3<double> operator*(const vec3<double>& v, double t) {
		return t * v;
	}

	inline vec3<double> operator/(const vec3<double>& v, double t) {
		return (1 / t) * v;
	}

	inline double dot(const vec3<double>& u, const vec3<double>& v) {
		return u.e[0] * v.e[0]
			+ u.e[1] * v.e[1]
			+ u.e[2] * v.e[2];
	}

	inline vec3<double> cross(const vec3<double>& u, const vec3<double>& v) {
		return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
			u.e[2] * v.e[0] - u.e[0] * v.e[2],
			u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	inline vec3<double> unit_vector(const vec3<double>& v) {
		return v / v.length();
	}

	inline vec3<double> random_unit_vector() {
		while (true) {
			auto p = vec3<double>::random(-1, 1);
			auto lensq = p.length_squared();
			if (1e-160 < lensq && lensq <= 1)
				return p / sqrt(lensq);
		}
	}

	inline vec3<double> random_on_hemisphere(const vec3<double>& normal) {
		vec3<double> on_unit_sphere = random_unit_vector();
		if (dot(on_unit_sphere, normal) > 0.0) // in the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}

	inline vec3<double> reflect(const vec3<double>& v, const vec3<double>& n) {
		return v - 2 * dot(v, n) * n;
	}
}
