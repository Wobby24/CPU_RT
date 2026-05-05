#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <type_traits>
#include <memory>

// tutorial has an 'using std' for shared pointers and maked shared functions, but its not the best practice to have that in a common for naming clarity reasons
// I also stripped the header of common headers for the sake of being explicit; I might need to add stuff later to this, but right now its pretty bare bones

// Constant values
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Util functions
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline std::mt19937& global_rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

// Default: range [0, 1]
template <typename T>
inline T random_value() {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(0, 1);
        return dist(global_rng());
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(0, 1);
        return dist(global_rng());
    }
}

// Range [min, max]
template <typename T>
inline T random_value(T min, T max) {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(global_rng());
    }
    else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(global_rng());
    }
}